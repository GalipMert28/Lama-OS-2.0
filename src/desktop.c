#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "fat32.h"  // FAT32 dosya sistemi entegrasyonu için

// Masaüstü arka plan rengi
#define DESKTOP_BG_COLOR "#ADD8E6"

// Üst çubuk rengi
#define TOP_BAR_COLOR "#000000"

// Gerçek zamanlı saat güncellemesi için zaman aşımı (ms)
#define CLOCK_UPDATE_INTERVAL 1000

// Uygulama tarama aralığı (ms)
#define APP_SCAN_INTERVAL 5000

// Masaüstü buton boyutları
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 50

// Global değişkenler
GtkWidget *desktop;          // Masaüstü alanı
GtkWidget *clock_label;      // Saat etiketi
pthread_mutex_t desktop_mutex = PTHREAD_MUTEX_INITIALIZER; // Masaüstü için thread güvenliği

// Gerçek zamanlı saat güncelleme fonksiyonu
gboolean update_clock(gpointer data) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    // Mevcut zamanı al
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Zamanı biçimlendir
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);

    // Saat etiketini güncelle
    gtk_label_set_text(GTK_LABEL(clock_label), buffer);

    // Zaman aşımını devam ettir
    return TRUE;
}

// Butonlara tıklandığında çalışacak fonksiyon
void on_button_clicked(GtkButton *button, gpointer user_data) {
    const char *app_name = (const char *)user_data;
    char command[256];

    // Uygulamayı çalıştır (systemd servisi ve pid dosyası kullanarak)
    snprintf(command, sizeof(command), "systemctl --no-block start elf_monitor.service && echo %s > /var/run/elf_monitor.pid", app_name);
    system(command);

    printf("%s uygulaması çalıştırıldı.\n", app_name);
}

// Masaüstüne buton ekleme fonksiyonu
void add_desktop_button(const char *label, int x, int y) {
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_fixed_put(GTK_FIXED(desktop), button, x, y);
    gtk_widget_set_size_request(button, BUTTON_WIDTH, BUTTON_HEIGHT);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), (gpointer)label);
    gtk_widget_show(button);
}

// Masaüstünden buton silme fonksiyonu
void remove_desktop_buttons() {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(desktop));

    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }

    g_list_free(children);
}

// FAT32 dosya sistemini tarayarak uygulamaları bul ve masaüstüne ekle
void *scan_applications(void *data) {
    while (1) {
        // FAT32 dosya sistemini başlat
        if (fat32_init() != 0) {
            fprintf(stderr, "FAT32 başlatılamadı!\n");
            sleep(APP_SCAN_INTERVAL / 1000);
            continue;
        }

        // Kök dizindeki dosyaları oku
        FileInfo *files = fat32_read_directory("/");
        if (files == NULL) {
            fprintf(stderr, "Dosyalar okunamadı!\n");
            sleep(APP_SCAN_INTERVAL / 1000);
            continue;
        }

        // Masaüstü butonlarını güncellemeden önce kilitle
        pthread_mutex_lock(&desktop_mutex);

        // Mevcut butonları temizle
        remove_desktop_buttons();

        // Dosyaları tarayarak .exe uzantılı olanları bul ve buton ekle
        int x = 50, y = 50;
        for (int i = 0; files[i].name != NULL; i++) {
            if (strstr(files[i].name, ".exe") != NULL) {
                add_desktop_button(files[i].name, x, y);
                y += BUTTON_HEIGHT + 10; // Butonları dikey olarak yerleştir
            }
        }

        // Masaüstü butonlarını güncelleme işlemi tamamlandı, kilidi aç
        pthread_mutex_unlock(&desktop_mutex);

        // Belleği temizle
        fat32_free_file_info(files);

        // Tarama aralığı kadar bekle
        sleep(APP_SCAN_INTERVAL / 1000);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *top_bar;
    GtkWidget *top_bar_box;
    pthread_t scan_thread;

    // GTK başlat
    gtk_init(&argc, &argv);

    // Ana pencere oluştur
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "RTOS Masaüstü");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    gtk_window_maximize(GTK_WINDOW(window)); // Tam ekran yap
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Dikey kutu oluştur
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Üst çubuk oluştur
    top_bar = gtk_event_box_new();
    GdkColor top_bar_color;
    gdk_color_parse(TOP_BAR_COLOR, &top_bar_color);
    gtk_widget_modify_bg(top_bar, GTK_STATE_NORMAL, &top_bar_color);
    gtk_box_pack_start(GTK_BOX(vbox), top_bar, FALSE, FALSE, 0);

    // Üst çubuk içeriği
    top_bar_box = gtk_hbox_new(FALSE, 10);
    gtk_container_add(GTK_CONTAINER(top_bar), top_bar_box);

    // Saat etiketi oluştur
    clock_label = gtk_label_new("");
    gtk_box_pack_end(GTK_BOX(top_bar_box), clock_label, FALSE, FALSE, 10);
    gtk_widget_show(clock_label);

    // Saati güncellemek için zaman aşımı ekle
    g_timeout_add(CLOCK_UPDATE_INTERVAL, update_clock, NULL);

    // Masaüstü alanı oluştur
    desktop = gtk_fixed_new();
    GdkColor desktop_color;
    gdk_color_parse(DESKTOP_BG_COLOR, &desktop_color);
    gtk_widget_modify_bg(desktop, GTK_STATE_NORMAL, &desktop_color);
    gtk_box_pack_start(GTK_BOX(vbox), desktop, TRUE, TRUE, 0);

    // Uygulama tarama thread'ini başlat
    pthread_create(&scan_thread, NULL, scan_applications, NULL);

    // Pencereyi göster
    gtk_widget_show_all(window);

    // GTK ana döngüsünü başlat
    gtk_main();

    // Thread'i temizle
    pthread_cancel(scan_thread);
    pthread_join(scan_thread, NULL);

    return 0;
}
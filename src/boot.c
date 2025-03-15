#include <stdint.h>
#include <string.h>
#include "disk_io.h"
#include "fat32.h"

// Sabitler
#define KERNEL_LOAD_ADDRESS 0x10000  // Kernel'ı yüklemek için bellek adresi

// Ekran yazdırma fonksiyonları (VGA)
#define VGA_PORT 0xB8000 // VGA ekran belleği adresi

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" :: "a"(value), "Nd"(port));
}

void write_char(char c, uint16_t x, uint16_t y) {
    uint16_t* video_memory = (uint16_t*)VGA_PORT;
    video_memory[y * 80 + x] = (0x0F << 8) | c;  // 0x0F: beyaz renk, karakter
}

void clear_screen() {
    uint16_t* video_memory = (uint16_t*)VGA_PORT;
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = (0x0F << 8) | ' ';  // Ekranı temizle
    }
}

void print(const char* str) {
    while (*str) {
        write_char(*str, 0, 0);  // Yalnızca başlangıçta ekrana yazalım
        str++;
    }
}

// Kernel'ı bellek adresine yüklemek için yardımcı fonksiyon
void load_kernel(uint32_t kernel_sector, uint32_t kernel_size) {
    uint8_t buffer[SECTOR_SIZE]; // Veriyi tutacak geçici bir buffer
    uint32_t kernel_address = KERNEL_LOAD_ADDRESS;

    // Kernel'ı diskin FAT32 dosya sisteminden okuma işlemi
    for (uint32_t sector = kernel_sector; kernel_size > 0; kernel_size -= SECTOR_SIZE, sector++) {
        // FAT32 üzerinden diske erişim
        if (read_sector(sector, buffer) != 0) {
            print("Disk okuma hatası!\n");
            return;
        }

        // Okunan veriyi bellek adresine kopyala
        for (int i = 0; i < SECTOR_SIZE && kernel_size > 0; i++) {
            *((uint8_t*)kernel_address++) = buffer[i];
            kernel_size--;
        }
    }

    // Kernel yüklendi, kernel'ı başlatmak için kontrolü kernel'a veriyoruz
    print("Kernel yuklendi, baslatiliyor...\n");
    void (*kernel_main)(void) = (void*)KERNEL_LOAD_ADDRESS;
    kernel_main();
}

// `kernel.c` dosyasını FAT32 dosya sisteminde arar ve yükler
void load_kernel_from_fat32() {
    // FAT32 ile dosya sistemine erişim için ilk olarak kök dizini aramamız gerekiyor
    DIR_ENTRY kernel_entry = {0};
    if (find_file("kernel.c", &kernel_entry) != 0) {
        print("kernel.c dosyasi bulunamadi!\n");
        return;
    }

    // Kernel dosyasının sektörünü alalım ve yükleyelim
    uint32_t kernel_sector = kernel_entry.first_cluster;  // FAT32'de ilk cluster bilgisi
    uint32_t kernel_size = kernel_entry.file_size; // Dosyanın boyutu

    // Kernel'ı yükle
    load_kernel(kernel_sector, kernel_size);
}

// Gerçek zamanlı boot işlemi başlat
void boot() {
    clear_screen();  // Ekranı temizle
    print("Bootloader Calisiyor...\n");

    // FAT32 dosya sisteminden kernel.c dosyasını yükle
    load_kernel_from_fat32();
}

int main() {
    // Boot işlemi başlatılıyor
    boot();

    // Bu noktadan sonra kernel çalışmaya başladığında bu döngü çalışmaz.
    while (1) {
        // Hiçbir şey yapma; kernel'a kontrol verilecek
    }

    return 0;
}

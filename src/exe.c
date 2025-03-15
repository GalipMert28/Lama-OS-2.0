#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>
#include <dirent.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>

#define SLEEP_TIME 5
#define LOG_FILE "/var/log/exe_monitor.log"

volatile sig_atomic_t running = 1;

void signal_handler(int signum) {
    running = 0;
}

void analyze_elf64(const char *filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) return;

    Elf64_Ehdr header;
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        close(fd);
        return;
    }

    // ELF dosyası kontrolü
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) == 0 && 
        header.e_ident[EI_CLASS] == ELFCLASS64) {
        
        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

        // Log dosyasına yazma
        FILE *log_file = fopen(LOG_FILE, "a");
        if (log_file) {
            fprintf(log_file, "[%s] Tespit edilen x64 ELF: %s\n", timestamp, filepath);
            fprintf(log_file, "Entry Point: 0x%lx\n", header.e_entry);
            fprintf(log_file, "Section sayisi: %d\n", header.e_shnum);
            fprintf(log_file, "Program header sayisi: %d\n\n", header.e_phnum);
            fclose(log_file);
        }

        // Sistem log'una yazma
        syslog(LOG_INFO, "X64 ELF tespit edildi: %s", filepath);
    }

    close(fd);
}

void scan_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular dosya
            char filepath[PATH_MAX];
            snprintf(filepath, PATH_MAX, "%s/%s", path, entry->d_name);
            analyze_elf64(filepath);
        }
        else if (entry->d_type == DT_DIR && 
                 strcmp(entry->d_name, ".") != 0 && 
                 strcmp(entry->d_name, "..") != 0) {
            char newpath[PATH_MAX];
            snprintf(newpath, PATH_MAX, "%s/%s", path, entry->d_name);
            scan_directory(newpath);
        }
    }
    closedir(dir);
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    
    pid_t sid = setsid();
    if (sid < 0) exit(EXIT_FAILURE);

    if (chdir("/") < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    // Daemon process oluştur
    daemonize();

    // Signal handler kayıt
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // Syslog başlat
    openlog("elf_monitor", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "ELF monitor başlatıldı");

    // PID dosyası oluştur
    FILE *pid_file = fopen("/var/run/elf_monitor.pid", "w");
    if (pid_file) {
        fprintf(pid_file, "%d", getpid());
        fclose(pid_file);
    }

    // Ana monitoring döngüsü
    while (running) {
        scan_directory("/");  // Root dizinden başla
        sleep(SLEEP_TIME);    // 5 saniye bekle
    }

    // Temizlik
    syslog(LOG_INFO, "ELF monitor durduruldu");
    closelog();
    unlink("/var/run/elf_monitor.pid");

    return EXIT_SUCCESS;
}

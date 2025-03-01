#include <stdio.h>
#include <stdlib.h>

void extract_tar_gz(const char *tar_gz_file) {
    char command[256];
    snprintf(command, sizeof(command), "tar -xzf %s", tar_gz_file);
    system(command);  // Shell komutunu çalıştır
}

void start_python(const char *python_script) {
    char command[256];
    snprintf(command, sizeof(command), "python3 %s", python_script);
    system(command);  // Python yorumlayıcısını çalıştırarak kernel.py'yi başlat
}

void kernel_main(void) {
    // Ps.bin başlatıldı
    printf("PS.BIN Kernel Başladı...\n");

    // Tar.gz dosyasını çıkartma (örneğin, kernel.py'yi içeren dosya)
    extract_tar_gz("/mnt/pytcod.tar.gz");

    // Kernel.py dosyasını çalıştırma
    start_python("/mnt/kernel.py");

    // Sonsuz döngü - Kernel arka planda çalışmaya devam eder
    while(1) { }
}

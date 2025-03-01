#include "fsinfo.h"

// FSINFO sektörünü oku ve root cluster'a erişim sağla
void read_fsinfo(uint32_t lba, fat32_boot_sector_t* boot_sector) {
    uint32_t fsinfo_sector = boot_sector->fs_info_sector;
    uint8_t buffer[512];

    // FSINFO sektörünü oku
    read_sectors(fsinfo_sector + lba, buffer, 1);

    // Root cluster'ı ve FAT bilgilerini burada işleyebilirsiniz
    // fsinfo içinde başka işlemler de yapılabilir.
}

#include "fat32.h"
#include "disk_io.h"

// FAT32 boot sektörünü oku
void read_fat32_boot_sector(uint32_t lba, fat32_boot_sector_t* boot_sector) {
    // Diskten FAT32 boot sektörünü oku (ilk sektör)
    read_sectors(lba, (uint8_t*) boot_sector, 1);
}

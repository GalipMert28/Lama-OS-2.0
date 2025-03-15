#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#pragma pack(push, 1)  // Yapıları hizalamak için
typedef struct {
    uint8_t  jump[3];           // Jump instruction (bootloader komutları)
    uint8_t  oem_id[8];         // OEM id
    uint16_t bytes_per_sector;  // Sektör başına byte sayısı
    uint8_t  sectors_per_cluster; // Bir cluster başına sektör sayısı
    uint16_t reserved_sector_count; // Ayrılmış sektör sayısı
    uint8_t  fat_count;         // FAT tablosu sayısı
    uint16_t root_dir_entries;  // Kök dizinindeki dosya sayısı
    uint16_t total_sectors;     // Toplam sektör sayısı
    uint8_t  media_type;        // Disk tipi
    uint16_t fat_size;          // FAT tablosunun boyutu (sektör olarak)
    uint16_t sectors_per_track; // Bir izdeki sektör sayısı
    uint16_t head_count;        // Başlık sayısı
    uint32_t hidden_sectors;    // Gizli sektörler (genellikle boot sektöründen önceki)
    uint32_t total_sectors_large; // Toplam sektör sayısı (genişletilmiş)
    uint32_t fat32_root_cluster;  // Root dizininin başladığı cluster
    uint16_t fs_info_sector;    // FSINFO sektör numarası
    uint16_t backup_boot_sector; // Yedek boot sektörü
} __attribute__((packed)) fat32_boot_sector_t;
#pragma pack(pop)

void read_fat32_boot_sector(uint32_t lba, fat32_boot_sector_t* boot_sector);

#endif

#ifndef FSINFO_H
#define FSINFO_H

#include "fat32.h"

void read_fsinfo(uint32_t lba, fat32_boot_sector_t* boot_sector);

#endif

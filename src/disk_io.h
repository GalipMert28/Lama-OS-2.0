#ifndef DISK_IO_H
#define DISK_IO_H

#include <stdint.h>

void read_sectors(uint32_t lba, uint8_t* buffer, uint32_t sectors);

#endif

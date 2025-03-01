#include "disk_io.h"

// BIOS disk okuma servisini kullanarak diske veri okuma
void read_sectors(uint32_t lba, uint8_t* buffer, uint32_t sectors) {
    __asm__ __volatile__ (
        "movl %[sectors], %%ecx;"     // Okunacak sektör sayısını ECX'e yerleştir
        "movl %[lba], %%ebx;"         // LBA'yı EBX'e yerleştir
        "movl %[buffer], %%edi;"      // Buffer adresini EDI'ye yerleştir
        "movb $0x02, %%al;"           // Diskin okuma komutu (0x02)
        "int $0x13;"                  // BIOS Disk servisini çağır
        :
        : [lba] "r" (lba), [buffer] "r" (buffer), [sectors] "r" (sectors)
        : "eax", "ebx", "ecx", "edx", "edi", "esi"
    );
}

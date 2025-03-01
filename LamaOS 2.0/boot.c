#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint16_t*) 0xB8000
#define PS_BIN_ADDRESS 0x100000  // PS.BIN'in yükleneceği bellek adresi

static uint16_t* vga_buffer = (uint16_t*) VGA_MEMORY;

// VGA yazma fonksiyonu
static void print_char(char c, uint8_t color, int x, int y) {
    uint16_t *location = vga_buffer + y * VGA_WIDTH + x;
    *location = (color << 8) | c;
}

static void print_string(const char* str, uint8_t color, int x, int y) {
    while (*str) {
        print_char(*str++, color, x++, y);
        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }
    }
}

// Diskten okuma işlemi için BIOS servisleri kullanılacak
void read_sectors(uint32_t lba, uint8_t* buffer, uint32_t sectors) {
    __asm__ __volatile__ (
        "movl %[sectors], %%ecx;"
        "movl %[lba], %%ebx;"
        "movl %[buffer], %%edi;"
        "movb $0x02, %%al;"  // Diskin okuma komutu
        "int $0x13;"          // BIOS Disk servis çağrısı
        :
        : [lba] "r" (lba), [buffer] "r" (buffer), [sectors] "r" (sectors)
        : "eax", "ebx", "ecx", "edx", "edi", "esi"
    );
}

// Kernel'in ana işlevi
void kernel_main(void) {
    int x = 0, y = 0;
    
    // Boot mesajını yazdırma
    print_string("Booting ps.bin...\n", 0x0F, x, y);

    // PS.BIN'in diskten okunması
    uint8_t* buffer = (uint8_t*) PS_BIN_ADDRESS;
    print_string("Reading ps.bin from disk...\n", 0x0F, x, y + 2);

    // Diskten okuma işlemi: 0. LBA'dan 1 sektör oku (örnek olarak)
    read_sectors(0, buffer, 1);  // Örneğin 0. sektörü oku

    // Sonsuz döngü, PS.BIN'in başarılı şekilde yüklendiğini varsayalım
    while (1) {
        // PS.BIN'in çalıştırılması burada yapılabilir
        // Ancak, bu aşamada bootloader sadece okuma yapacak
    }
}

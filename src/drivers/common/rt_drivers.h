/**
 * @file rt_drivers.h
 * @brief Gerçek zamanlı sürücü sistemi için ana başlık dosyası
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef RT_DRIVERS_H
#define RT_DRIVERS_H

#include <stdint.h>
#include <stdbool.h>
#include "../common/rt_types.h"

/* ================ GENEL TANIMLAMALAR ================ */

// Sürücü versiyonu
#define RT_DRIVERS_VERSION_MAJOR    1
#define RT_DRIVERS_VERSION_MINOR    0
#define RT_DRIVERS_VERSION_PATCH    0

// Sistem limitleri
#define MAX_DRIVERS                 32
#define MAX_INTERRUPTS             256
#define MAX_DMA_CHANNELS           16
#define MAX_IO_PORTS              1024

/* ================ INTERRUPT VEKTÖRÜ ================ */

// Donanım kesmeleri
#define IRQ_TIMER                   0
#define IRQ_KEYBOARD               1
#define IRQ_CASCADE                2
#define IRQ_COM2                   3
#define IRQ_COM1                   4
#define IRQ_LPT2                   5
#define IRQ_FLOPPY                 6
#define IRQ_LPT1                   7
#define IRQ_RTC                    8
#define IRQ_MOUSE                  12
#define IRQ_FPU                    13
#define IRQ_ATA1                   14
#define IRQ_ATA2                   15

/* ================ ÖNCELIK SEVIYELERI ================ */

typedef enum {
    RT_PRIORITY_REALTIME = 0,      // En yüksek öncelik (kesme işleyicileri)
    RT_PRIORITY_CRITICAL = 1,      // Kritik sistem işlemleri
    RT_PRIORITY_HIGH     = 2,      // Yüksek öncelikli sürücüler
    RT_PRIORITY_NORMAL   = 3,      // Normal sürücü işlemleri
    RT_PRIORITY_LOW      = 4,      // Arkaplan işlemleri
    RT_PRIORITY_IDLE     = 5       // En düşük öncelik
} RT_Priority;

/* ================ ZAMAN YÖNETİMİ ================ */

typedef struct {
    uint64_t seconds;              // Unix timestamp
    uint32_t milliseconds;         // Milisaniye
    uint32_t microseconds;         // Mikrosaniye
    uint32_t nanoseconds;          // Nanosaniye
} RT_TimeStamp;

/* ================ BELLEK YÖNETİMİ ================ */

typedef struct {
    void* physical_addr;           // Fiziksel bellek adresi
    void* virtual_addr;            // Sanal bellek adresi
    size_t size;                   // Bellek boyutu
    uint32_t flags;                // Bellek özellikleri
} RT_MemoryRegion;

/* ================ DMA YÖNETİMİ ================ */

typedef struct {
    uint32_t channel;              // DMA kanal numarası
    void* buffer;                  // DMA buffer adresi
    size_t size;                   // Transfer boyutu
    bool direction;                // Transfer yönü (0: read, 1: write)
    void (*callback)(void*);       // Transfer tamamlandığında çağrılacak fonksiyon
} RT_DMATransfer;

/* ================ SÜRÜCÜ DURUMLARI ================ */

typedef enum {
    DRIVER_STATE_UNINITIALIZED = 0,// Başlatılmamış
    DRIVER_STATE_INITIALIZING,     // Başlatılıyor
    DRIVER_STATE_READY,            // Hazır
    DRIVER_STATE_RUNNING,          // Çalışıyor
    DRIVER_STATE_STOPPING,         // Durduruluyor
    DRIVER_STATE_STOPPED,          // Durdurulmuş
    DRIVER_STATE_ERROR,            // Hata durumu
    DRIVER_STATE_SUSPENDED         // Askıya alınmış
} RT_DriverState;

/* ================ HATA KODLARI ================ */

typedef enum {
    RT_SUCCESS = 0,                // Başarılı
    RT_ERROR_INVALID_PARAMETER,    // Geçersiz parametre
    RT_ERROR_NOT_INITIALIZED,      // Başlatılmamış
    RT_ERROR_ALREADY_INITIALIZED,  // Zaten başlatılmış
    RT_ERROR_NO_MEMORY,           // Bellek yetersiz
    RT_ERROR_HARDWARE_FAULT,      // Donanım hatası
    RT_ERROR_TIMEOUT,             // Zaman aşımı
    RT_ERROR_BUSY,                // Meşgul
    RT_ERROR_NOT_SUPPORTED,       // Desteklenmiyor
    RT_ERROR_IO_ERROR             // I/O hatası
} RT_ErrorCode;

/* ================ SÜRÜCÜ YAPISI ================ */

typedef struct RT_Driver {
    // Temel bilgiler
    uint32_t id;                   // Sürücü ID
    const char* name;              // Sürücü adı
    const char* version;           // Sürücü versiyonu
    RT_DriverState state;          // Sürücü durumu
    RT_Priority priority;          // Sürücü önceliği
    
    // Donanım kaynakları
    uint32_t irq_number;           // Kesme numarası
    uint32_t io_port_base;         // I/O port başlangıç adresi
    RT_MemoryRegion* memory;       // Bellek bölgesi
    
    // Sürücü fonksiyonları
    RT_ErrorCode (*init)(struct RT_Driver*);            // Başlatma
    RT_ErrorCode (*start)(struct RT_Driver*);           // Çalıştırma
    RT_ErrorCode (*stop)(struct RT_Driver*);            // Durdurma
    RT_ErrorCode (*suspend)(struct RT_Driver*);         // Askıya alma
    RT_ErrorCode (*resume)(struct RT_Driver*);          // Devam ettirme
    void (*interrupt_handler)(struct RT_Driver*);        // Kesme işleyici
    
    // İstatistikler
    struct {
        uint64_t interrupts_handled;    // İşlenen kesme sayısı
        uint64_t bytes_transferred;     // Transfer edilen veri miktarı
        uint64_t errors_encountered;    // Karşılaşılan hata sayısı
        RT_TimeStamp last_error_time;   // Son hata zamanı
        RT_TimeStamp last_active_time;  // Son aktivite zamanı
    } stats;
    
    // Özel veriler
    void* private_data;            // Sürücüye özel veri yapısı
} RT_Driver;

/* ================ SÜRÜCÜ YÖNETİMİ FONKSİYONLARI ================ */

// Sürücü sistemi başlatma
RT_ErrorCode RT_InitializeDriverSystem(void);

// Yeni sürücü kaydetme
RT_ErrorCode RT_RegisterDriver(RT_Driver* driver);

// Sürücü kaldırma
RT_ErrorCode RT_UnregisterDriver(RT_Driver* driver);

// Sürücü bulma
RT_Driver* RT_FindDriver(const char* name);

// Tüm sürücüleri listeleme
void RT_ListDrivers(void);

/* ================ KESME YÖNETİMİ ================ */

// Kesme kaydetme
RT_ErrorCode RT_RegisterInterrupt(uint32_t irq, void (*handler)(void*), void* data);

// Kesme kaldırma
RT_ErrorCode RT_UnregisterInterrupt(uint32_t irq);

// Kesmeleri aktif/pasif yapma
void RT_EnableInterrupts(void);
void RT_DisableInterrupts(void);

/* ================ DMA FONKSİYONLARI ================ */

// DMA transfer başlatma
RT_ErrorCode RT_StartDMATransfer(RT_DMATransfer* transfer);

// DMA transfer durumu sorgulama
bool RT_IsDMATransferComplete(uint32_t channel);

/* ================ ZAMAN FONKSİYONLARI ================ */

// Sistem zamanı alma
void RT_GetSystemTime(RT_TimeStamp* timestamp);

// Mikrosaniye cinsinden bekleme
void RT_MicroDelay(uint32_t microseconds);

/* ================ DEBUG FONKSİYONLARI ================ */

#ifdef RT_DEBUG
void RT_DebugPrint(const char* format, ...);
void RT_DumpDriverInfo(RT_Driver* driver);
#endif

#endif // RT_DRIVERS_H

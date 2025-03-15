/**
 * @file io_port.h
 * @brief Donanım port I/O işlemleri için fonksiyonlar ve tanımlamalar
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef IO_PORT_H
#define IO_PORT_H

#include "rt_types.h"

/* ================ PORT TANIMLAMALARI ================ */

// Port I/O erişim tipi
typedef enum {
    IO_PORT_ACCESS_8BIT,  // 8-bit erişim
    IO_PORT_ACCESS_16BIT, // 16-bit erişim
    IO_PORT_ACCESS_32BIT  // 32-bit erişim
} IO_PortAccessType;

// Port I/O mode
typedef enum {
    IO_PORT_MODE_INPUT,   // Giriş modu
    IO_PORT_MODE_OUTPUT,  // Çıkış modu
    IO_PORT_MODE_BIDIR    // Çift yönlü mod
} IO_PortMode;

/* ================ PORT YAPISI ================ */

typedef struct {
    RT_IOPort     base;           // Portun temel adresi
    RT_U32        count;          // Port sayısı
    IO_PortAccessType access_type; // Erişim tipi
    IO_PortMode   mode;           // Port modu
    RT_Bool       locked;         // Port kilit durumu
} IO_PortConfig;

/* ================ PORT FONKSİYONLARI ================ */

// Port başlatma
RT_ErrorCode IO_InitPort(IO_PortConfig* config);

// Portu kilitleme
RT_ErrorCode IO_LockPort(RT_IOPort port);

// Port kilidini açma
RT_ErrorCode IO_UnlockPort(RT_IOPort port);

// 8-bit port okuma
RT_U8 IO_In8(RT_IOPort port);

// 8-bit port yazma
void IO_Out8(RT_IOPort port, RT_U8 value);

// 16-bit port okuma
RT_U16 IO_In16(RT_IOPort port);

// 16-bit port yazma
void IO_Out16(RT_IOPort port, RT_U16 value);

// 32-bit port okuma
RT_U32 IO_In32(RT_IOPort port);

// 32-bit port yazma
void IO_Out32(RT_IOPort port, RT_U32 value);

/* ================ PORT YÖNETİMİ ================ */

// Port konfigürasyonunu alma
IO_PortConfig* IO_GetPortConfig(RT_IOPort port);

// Port modunu ayarlama
RT_ErrorCode IO_SetPortMode(RT_IOPort port, IO_PortMode mode);

// Port erişim tipini ayarlama
RT_ErrorCode IO_SetPortAccessType(RT_IOPort port, IO_PortAccessType access_type);

/* ================ HATA KODLARI ================ */

#define IO_ERROR_PORT_LOCKED        0x1001
#define IO_ERROR_INVALID_PORT       0x1002
#define IO_ERROR_PORT_NOT_INITIALIZED 0x1003

#endif // IO_PORT_H

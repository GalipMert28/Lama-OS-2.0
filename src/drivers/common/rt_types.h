/**
 * @file rt_types.h
 * @brief Gerçek zamanlı sistemler için temel veri tipleri
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef RT_TYPES_H
#define RT_TYPES_H

#include <stdint.h>

/* ================ STANDART TİPLER ================ */

typedef uint8_t    RT_U8;
typedef uint16_t   RT_U16;
typedef uint32_t   RT_U32;
typedef uint64_t   RT_U64;

typedef int8_t     RT_S8;
typedef int16_t    RT_S16;
typedef int32_t    RT_S32;
typedef int64_t    RT_S64;

typedef float      RT_F32;
typedef double     RT_F64;

typedef char       RT_Char;
typedef bool       RT_Bool;

typedef void*      RT_Ptr;
typedef const void* RT_ConstPtr;

/* ================ MİMARİYE ÖZGÜ TİPLER ================ */

#ifdef __x86_64__
    typedef RT_U64 RT_Size;
    typedef RT_S64 RT_SSize;
#else
    typedef RT_U32 RT_Size;
    typedef RT_S32 RT_SSize;
#endif

/* ================ DONANIM ADRES TİPLERİ ================ */

typedef RT_U32     RT_IOPort;          // IO Port adresi
typedef RT_U64     RT_PhysAddr;        // Fiziksel bellek adresi
typedef RT_U64     RT_VirtAddr;        // Sanal bellek adresi

/* ================ ZAMAN TİPLERİ ================ */

typedef RT_U64     RT_ClockTicks;      // Zamanlayıcı tik sayısı
typedef RT_U32     RT_Microseconds;    // Mikrosaniye
typedef RT_U64     RT_Nanoseconds;     // Nanosaniye

/* ================ HATA VE DURUM TİPLERİ ================ */

typedef enum {
    RT_FALSE = 0,
    RT_TRUE = 1
} RT_Boolean;

/* ================ POINTER TİPLERİ ================ */

typedef struct {
    RT_PhysAddr physical;
    RT_VirtAddr virtual;
} RT_MappedAddr;

/* ================ DONANIM ÖZEL TİPLER ================ */

typedef RT_U32     RT_IRQNumber;       // Kesme numarası
typedef RT_U32     RT_DMAChannel;      // DMA kanalı

/* ================ VERİ YAPILARI ================ */

typedef struct {
    RT_U32     id;                     // Benzersiz kimlik
    RT_Char    name[32];               // İsim alanı
    RT_U32     flags;                  // Özellik bayrakları
} RT_Identifier;

typedef struct {
    RT_U16     major;                  // Ana sürüm
    RT_U16     minor;                  // Alt sürüm
    RT_U16     patch;                  // Düzeltme
} RT_Version;

/* ================ TEMEL FONKSİYON TİPLERİ ================ */

typedef void (*RT_ISRHandler)(RT_Ptr); // Kesme servis yordamı
typedef void (*RT_Callback)(RT_Ptr);   // Geri çağırım fonksiyonu

/* ================ GENEL DEĞERLER ================ */

#define RT_NULL         (0x0)          // Geçersiz gösterici
#define RT_INVALID_ID   (0xFFFFFFFF)   // Geçersiz kimlik değeri

#endif // RT_TYPES_H

/**
 * @file mouse_driver.h
 * @brief PS/2 Fare Sürücüsü
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef MOUSE_DRIVER_H
#define MOUSE_DRIVER_H

#include "common/rt_drivers.h"
#include "common/io_port.h"

/* ================ MOUSE KOMUTLARI ================ */

#define MOUSE_CMD_RESET             0xFF
#define MOUSE_CMD_ENABLE            0xF4
#define MOUSE_CMD_DISABLE           0xF5
#define MOUSE_CMD_SET_SAMPLE_RATE   0xF3
#define MOUSE_CMD_SET_RESOLUTION    0xE8
#define MOUSE_CMD_GET_DEVICE_ID     0xF2
#define MOUSE_CMD_SET_SCALING_1_1   0xE6
#define MOUSE_CMD_SET_SCALING_2_1   0xE7
#define MOUSE_CMD_SET_STREAM_MODE   0xEA
#define MOUSE_CMD_SET_REMOTE_MODE   0xF0

/* ================ MOUSE REGISTERLERI ================ */

#define MOUSE_STATUS_REGISTER       0x64
#define MOUSE_DATA_REGISTER         0x60
#define MOUSE_COMMAND_REGISTER      0x64

/* ================ MOUSE PAKET YAPISI ================ */

#define MOUSE_PACKET_SIZE           3
#define MOUSE_PACKET_BUTTON_MASK    0x07
#define MOUSE_PACKET_X_SIGN_BIT     0x10
#define MOUSE_PACKET_Y_SIGN_BIT     0x20
#define MOUSE_PACKET_OVERFLOW_BIT   0xC0

/* ================ MOUSE MODLARI ================ */

typedef enum {
    MOUSE_MODE_STREAM,   // Sürekli veri gönderimi
    MOUSE_MODE_REMOTE    // Talep üzerine veri gönderimi
} MouseMode;

/* ================ MOUSE BUTONLARI ================ */

typedef enum {
    MOUSE_BUTTON_LEFT    = 0x01,
    MOUSE_BUTTON_RIGHT   = 0x02,
    MOUSE_BUTTON_MIDDLE  = 0x04,
    MOUSE_BUTTON_SIDE    = 0x08,
    MOUSE_BUTTON_EXTRA   = 0x10
} MouseButtons;

/* ================ MOUSE VERI YAPISI ================ */

typedef struct {
    RT_S16 x;                // X hareketi
    RT_S16 y;                // Y hareketi
    RT_S16 z;                // Scroll wheel
    MouseButtons buttons;    // Buton durumu
    RT_Bool moved;           // Hareket var mı?
} MouseData;

/* ================ MOUSE CALLBACK TİPİ ================ */

typedef void (*MouseCallback)(MouseData* data);

/* ================ MOUSE SÜRÜCÜ YAPISI ================ */

typedef struct {
    RT_Driver        base;            // Temel sürücü yapısı
    MouseData        data;            // Mouse verileri
    MouseMode        mode;            // Çalışma modu
    MouseCallback    callback;        // Veri geldiğinde çağrılacak fonksiyon
    RT_U8            packet[MOUSE_PACKET_SIZE]; // Ham veri paketi
    RT_U8            packet_index;    // Paket indeksi
    RT_Bool          packet_complete; // Paket tamamlandı mı?
} RT_MouseDriver;

/* ================ MOUSE FONKSİYONLARI ================ */

// Fare sürücüsünü başlatma
RT_ErrorCode Mouse_Init(RT_MouseDriver* driver);

// Fare sürücüsünü etkinleştirme
RT_ErrorCode Mouse_Enable(RT_MouseDriver* driver);

// Fare sürücüsünü devre dışı bırakma
RT_ErrorCode Mouse_Disable(RT_MouseDriver* driver);

// Kesme servis yordamı
void Mouse_InterruptHandler(RT_MouseDriver* driver);

// Fare verilerini işleme
void Mouse_ProcessPacket(RT_MouseDriver* driver);

// Fare parametrelerini ayarlama
RT_ErrorCode Mouse_SetSampleRate(RT_MouseDriver* driver, RT_U8 rate);

#endif // MOUSE_DRIVER_H

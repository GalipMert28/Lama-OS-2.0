/**
 * @file mouse_driver.c
 * @brief PS/2 Fare Sürücüsü Uygulaması
 * @version 1.0
 * @date 2025-03-15
 */

#include "mouse_driver.h"
#include "common/io_port.h"
#include "common/rt_types.h"

/* ================ LOCAL DEĞİŞKENLER ================ */

static RT_MouseDriver mouse_driver_instance = {
    .base = {
        .id = 1,
        .name = "PS/2 Mouse Driver",
        .version = "1.0",
        .state = DRIVER_STATE_UNINITIALIZED,
        .priority = RT_PRIORITY_HIGH,
        .irq_number = IRQ_MOUSE,
        .io_port_base = MOUSE_DATA_REGISTER,
        .interrupt_handler = (RT_ISRHandler)Mouse_InterruptHandler
    },
    .mode = MOUSE_MODE_STREAM,
    .packet_index = 0,
    .packet_complete = RT_FALSE
};

/* ================ STATİK FONKSİYONLAR ================ */

// Port hazır mı kontrol et
static RT_Bool Mouse_WaitInput(void) {
    RT_U32 timeout = 100000; // 100ms timeout
    while (timeout--) {
        if (!(IO_In8(MOUSE_STATUS_REGISTER) & STATUS_INPUT_FULL)) {
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}

static RT_Bool Mouse_WaitOutput(void) {
    RT_U32 timeout = 100000; // 100ms timeout
    while (timeout--) {
        if (IO_In8(MOUSE_STATUS_REGISTER) & STATUS_OUTPUT_FULL) {
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}

// 0xFA (ACK) yanıtını bekle
static RT_Bool Mouse_WaitAck(void) {
    RT_U8 response;
    if (!Mouse_WaitOutput()) {
        return RT_FALSE;
    }
    response = IO_In8(MOUSE_DATA_REGISTER);
    return response == 0xFA;
}

/* ================ GENEL FONKSİYONLAR ================ */

RT_ErrorCode Mouse_Init(RT_MouseDriver* driver) {
    if (!driver) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Komut göndermeden önce fareyi devre dışı bırak
    IO_Out8(MOUSE_COMMAND_REGISTER, 0xA7);

    // Fareyi resetle
    IO_Out8(MOUSE_DATA_REGISTER, MOUSE_CMD_RESET);

    // Reset ACK kontrolü
    if (!Mouse_WaitAck()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    // Self-test sonucunu bekle
    if (!Mouse_WaitOutput()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    RT_U8 self_test = IO_In8(MOUSE_DATA_REGISTER);
    if (self_test != 0xAA) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    // Fareyi aktifleştir
    IO_Out8(MOUSE_DATA_REGISTER, MOUSE_CMD_ENABLE);

    // ACK bekle
    if (!Mouse_WaitAck()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    // Varsayılan ayarları yap
    IO_Out8(MOUSE_DATA_REGISTER, MOUSE_CMD_SET_SAMPLE_RATE);
    IO_Out8(MOUSE_DATA_REGISTER, 100); // 100Hz örnekleme hızı

    // Paket indeksini sıfırla
    driver->packet_index = 0;

    driver->base.state = DRIVER_STATE_READY;
    return RT_SUCCESS;
}

RT_ErrorCode Mouse_Enable(RT_MouseDriver* driver) {
    if (!driver || driver->base.state != DRIVER_STATE_READY) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    IO_Out8(MOUSE_DATA_REGISTER, MOUSE_CMD_ENABLE);
    if (!Mouse_WaitAck()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    driver->base.state = DRIVER_STATE_RUNNING;
    return RT_SUCCESS;
}

RT_ErrorCode Mouse_Disable(RT_MouseDriver* driver) {
    if (!driver || driver->base.state != DRIVER_STATE_RUNNING) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    IO_Out8(MOUSE_DATA_REGISTER, MOUSE_CMD_DISABLE);
    if (!Mouse_WaitAck()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    driver->base.state = DRIVER_STATE_READY;
    return RT_SUCCESS;
}

void Mouse_InterruptHandler(RT_MouseDriver* driver) {
    if (!driver || driver->base.state != DRIVER_STATE_RUNNING) {
        return;
    }

    RT_U8 data = IO_In8(MOUSE_DATA_REGISTER);

    // Paket toplanması
    driver->packet[driver->packet_index++] = data;

    // İlk bayt alındı mı kontrol et
    if (driver->packet_index == 1) {
        if (!(driver->packet[0] & (MOUSE_PACKET_X_SIGN_BIT | MOUSE_PACKET_Y_SIGN_BIT))) {
            driver->packet_index = 0;
            return;
        }
    }

    // Tam paket alındı mı kontrol et
    if (driver->packet_index >= MOUSE_PACKET_SIZE) {
        driver->packet_complete = RT_TRUE;
        driver->packet_index = 0;
        Mouse_ProcessPacket(driver);
    }
}

void Mouse_ProcessPacket(RT_MouseDriver* driver) {
    if (!driver || !driver->packet_complete || driver->base.state != DRIVER_STATE_RUNNING) {
        return;
    }

    RT_U8 flags = driver->packet[0];

    // X ve Y hareketi (9-bit signed integers)
    RT_S16 x = driver->packet[1];
    if (flags & MOUSE_PACKET_X_SIGN_BIT) {
        x -= 256;
    }

    RT_S16 y = driver->packet[2];
    if (flags & MOUSE_PACKET_Y_SIGN_BIT) {
        y -= 256;
    }

    // Buton durumları
    driver->data.buttons = flags & MOUSE_PACKET_BUTTON_MASK;

    // X ve Y koordinatlarını güncelle
    driver->data.x += x;
    driver->data.y -= y; // Y koordinatı ters
    driver->data.moved = (x != 0 || y != 0);

    // Callback çağır
    if (driver->callback && driver->data.moved) {
        driver->callback(&driver->data);
    }

    driver->packet_complete = RT_FALSE;
}

RT_ErrorCode Mouse_SetSampleRate(RT_MouseDriver* driver, RT_U8 rate) {
    if (!driver || driver->base.state != DRIVER_STATE_READY) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    IO_Out8(MOUSE_DATA_REGISTER, MOUSE_CMD_SET_SAMPLE_RATE);
    IO_Out8(MOUSE_DATA_REGISTER, rate);

    if (!Mouse_WaitAck()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    driver->data.sample_rate = rate;
    return RT_SUCCESS;
}

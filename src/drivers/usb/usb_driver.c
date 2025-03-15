/**
 * @file usb_driver.c
 * @brief USB (Universal Serial Bus) Sürücüsü Uygulaması
 * @version 1.0
 * @date 2025-03-15
 */

#include "usb_driver.h"
#include "common/io_port.h"
#include "common/rt_types.h"
#include <string.h>

/* ================ LOCAL DEĞİŞKENLER ================ */

static RT_USBDriver usb_driver_instance = {
    .base = {
        .id = 3,
        .name = "USB Driver",
        .version = "1.0",
        .state = DRIVER_STATE_UNINITIALIZED,
        .priority = RT_PRIORITY_HIGH,
        .irq_number = IRQ_USB,
        .interrupt_handler = NULL
    },
    .num_devices = 0,
    .on_device_connected = NULL,
    .on_device_disconnected = NULL
};

/* ================ STATİK FONKSİYONLAR ================ */

// USB portu resetleme
static RT_ErrorCode USB_ResetPort(RT_USBDriver* driver) {
    // USB portunu resetle
    IO_Out16(0xCF8, 0x80001004); // USB kontrol registerı
    IO_Out16(0xCFC, 0x00000001); // Reset biti
    return RT_SUCCESS;
}

// USB cihaz tanımlayıcısını oku
static RT_ErrorCode USB_ReadDeviceDescriptor(RT_USBDriver* driver, RT_U8 address, USB_DeviceDescriptor* desc) {
    if (!driver || !desc) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Cihaz tanımlayıcısını oku
    IO_Out8(0xCF8, 0x80001000 | (address << 8)); // USB cihaz adresi
    IO_Out16(0xCFC, 0x00000001); // Cihaz tanımlayıcısı komutu
    memcpy(desc, (void*)0xE000, sizeof(USB_DeviceDescriptor)); // Tanımlayıcıyı kopyala
    return RT_SUCCESS;
}

/* ================ GENEL FONKSİYONLAR ================ */

RT_ErrorCode USB_Init(RT_USBDriver* driver) {
    if (!driver) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // USB portunu resetle
    RT_ErrorCode err = USB_ResetPort(driver);
    if (err != RT_SUCCESS) {
        return err;
    }

    driver->base.state = DRIVER_STATE_READY;
    return RT_SUCCESS;
}

USB_Device* USB_FindDevice(RT_USBDriver* driver, RT_U8 address) {
    if (!driver || address >= USB_MAX_DEVICES) {
        return NULL;
    }

    for (RT_U8 i = 0; i < driver->num_devices; i++) {
        if (driver->devices[i].address == address) {
            return &driver->devices[i];
        }
    }
    return NULL;
}

RT_ErrorCode USB_AttachDevice(RT_USBDriver* driver, USB_Device* device) {
    if (!driver || !device || driver->num_devices >= USB_MAX_DEVICES) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Cihaz tanımlayıcısını oku
    RT_ErrorCode err = USB_ReadDeviceDescriptor(driver, driver->num_devices, &device->device_desc);
    if (err != RT_SUCCESS) {
        return err;
    }

    // Cihazı listeye ekle
    driver->devices[driver->num_devices++] = *device;

    // Callback çağır
    if (driver->on_device_connected) {
        driver->on_device_connected(device);
    }

    return RT_SUCCESS;
}

RT_ErrorCode USB_DetachDevice(RT_USBDriver* driver, RT_U8 address) {
    if (!driver || address >= USB_MAX_DEVICES) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    for (RT_U8 i = 0; i < driver->num_devices; i++) {
        if (driver->devices[i].address == address) {
            // Cihazı listeden kaldır
            memmove(&driver->devices[i], &driver->devices[i + 1], (driver->num_devices - i - 1) * sizeof(USB_Device));
            driver->num_devices--;

            // Callback çağır
            if (driver->on_device_disconnected) {
                driver->on_device_disconnected(address);
            }

            return RT_SUCCESS;
        }
    }
    return RT_ERROR_INVALID_PARAMETER;
}

RT_ErrorCode USB_SendData(RT_USBDriver* driver, RT_U8 address, RT_U8 endpoint, RT_U8* data, RT_U32 length) {
    if (!driver || !data || length == 0) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Veriyi gönder
    IO_Out8(0xCF8, 0x80001000 | (address << 8) | (endpoint << 4));
    IO_Out16(0xCFC, 0x00000001); // Veri gönderme komutu
    IO_Out32(0xE000, length); // Veri boyutu
    IO_Out32(0xE004, (RT_U32)data); // Veri adresi
    return RT_SUCCESS;
}

RT_ErrorCode USB_ReceiveData(RT_USBDriver* driver, RT_U8 address, RT_U8 endpoint, RT_U8* buffer, RT_U32 length) {
    if (!driver || !buffer || length == 0) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Veriyi al
    IO_Out8(0xCF8, 0x80001000 | (address << 8) | (endpoint << 4));
    IO_Out16(0xCFC, 0x00000002); // Veri alma komutu
    IO_Out32(0xE000, length); // Veri boyutu
    IO_Out32(0xE004, (RT_U32)buffer); // Buffer adresi
    return RT_SUCCESS;
}

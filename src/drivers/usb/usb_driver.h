/**
 * @file usb_driver.h
 * @brief USB (Universal Serial Bus) Sürücüsü
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include "common/rt_drivers.h"
#include "common/rt_types.h"

/* ================ USB TANIMLAMALARI ================ */

#define USB_MAX_DEVICES            32  // Maksimum cihaz sayısı
#define USB_MAX_ENDPOINTS          16  // Maksimum endpoint sayısı
#define USB_MAX_INTERFACES         8   // Maksimum interface sayısı
#define USB_MAX_PACKET_SIZE        1024 // Maksimum paket boyutu

/* ================ USB TİPLERİ ================ */

typedef enum {
    USB_TYPE_DEVICE = 0,          // USB Cihaz
    USB_TYPE_HUB,                 // USB Hub
    USB_TYPE_HOST                 // USB Host
} USB_DeviceType;

typedef enum {
    USB_SPEED_LOW = 0,            // Low-Speed (1.5 Mbps)
    USB_SPEED_FULL,               // Full-Speed (12 Mbps)
    USB_SPEED_HIGH,               // High-Speed (480 Mbps)
    USB_SPEED_SUPER               // Super-Speed (5 Gbps)
} USB_Speed;

/* ================ USB DESCRIPTOR TİPLERİ ================ */

typedef enum {
    USB_DESC_DEVICE = 1,          // Cihaz tanımlayıcısı
    USB_DESC_CONFIGURATION,       // Konfigürasyon tanımlayıcısı
    USB_DESC_STRING,              // String tanımlayıcısı
    USB_DESC_INTERFACE,           // Arayüz tanımlayıcısı
    USB_DESC_ENDPOINT,            // Endpoint tanımlayıcısı
    USB_DESC_HID,                 // HID tanımlayıcısı
    USB_DESC_HUB                  // Hub tanımlayıcısı
} USB_DescriptorType;

/* ================ USB ENDPOINT TİPLERİ ================ */

typedef enum {
    USB_EP_CONTROL = 0,           // Kontrol endpointi
    USB_EP_ISOCHRONOUS,           // İzokron endpoint
    USB_EP_BULK,                  // Bulk endpoint
    USB_EP_INTERRUPT              // Kesme endpointi
} USB_EndpointType;

/* ================ USB DESCRIPTOR YAPILARI ================ */

typedef struct {
    RT_U8  length;
    RT_U8  descriptor_type;
    RT_U16 bcd_usb;
    RT_U8  device_class;
    RT_U8  device_sub_class;
    RT_U8  device_protocol;
    RT_U8  max_packet_size;
    RT_U16 vendor_id;
    RT_U16 product_id;
    RT_U16 device_version;
    RT_U8  manufacturer_index;
    RT_U8  product_index;
    RT_U8  serial_number_index;
    RT_U8  num_configurations;
} __attribute__((packed)) USB_DeviceDescriptor;

typedef struct {
    RT_U8 length;
    RT_U8 descriptor_type;
    RT_U16 total_length;
    RT_U8 num_interfaces;
    RT_U8 config_value;
    RT_U8 config_index;
    RT_U8 attributes;
    RT_U8 max_power;
} __attribute__((packed)) USB_ConfigDescriptor;

typedef struct {
    RT_U8 length;
    RT_U8 descriptor_type;
    RT_U8 interface_number;
    RT_U8 alternate_setting;
    RT_U8 num_endpoints;
    RT_U8 interface_class;
    RT_U8 interface_sub_class;
    RT_U8 interface_protocol;
    RT_U8 interface_index;
} __attribute__((packed)) USB_InterfaceDescriptor;

typedef struct {
    RT_U8 length;
    RT_U8 descriptor_type;
    RT_U8 endpoint_address;
    RT_U8 attributes;
    RT_U16 max_packet_size;
    RT_U8 interval;
} __attribute__((packed)) USB_EndpointDescriptor;

/* ================ USB CİHAZ YAPISI ================ */

typedef struct {
    USB_DeviceDescriptor device_desc;
    USB_ConfigDescriptor config_desc;
    USB_InterfaceDescriptor interface_desc[USB_MAX_INTERFACES];
    USB_EndpointDescriptor endpoint_desc[USB_MAX_ENDPOINTS];
    USB_DeviceType type;
    USB_Speed speed;
    RT_U8 address;
    RT_Bool connected;
} USB_Device;

/* ================ USB SÜRÜCÜ YAPISI ================ */

typedef struct {
    RT_Driver base;                  // Temel sürücü yapısı
    USB_Device devices[USB_MAX_DEVICES]; // Bağlı cihazlar
    RT_U8 num_devices;                // Bağlı cihaz sayısı
    void (*on_device_connected)(USB_Device*); // Cihaz bağlandığında çağrılacak fonksiyon
    void (*on_device_disconnected)(RT_U8); // Cihaz ayrıldığında çağrılacak fonksiyon
} RT_USBDriver;

/* ================ USB FONKSİYONLARI ================ */

// USB sürücüsü başlatma
RT_ErrorCode USB_Init(RT_USBDriver* driver);

// USB cihazı bulma
USB_Device* USB_FindDevice(RT_USBDriver* driver, RT_U8 address);

// USB cihazı bağlama
RT_ErrorCode USB_AttachDevice(RT_USBDriver* driver, USB_Device* device);

// USB cihazı ayırma
RT_ErrorCode USB_DetachDevice(RT_USBDriver* driver, RT_U8 address);

// USB veri gönderme
RT_ErrorCode USB_SendData(RT_USBDriver* driver, RT_U8 address, RT_U8 endpoint, RT_U8* data, RT_U32 length);

// USB veri alma
RT_ErrorCode USB_ReceiveData(RT_USBDriver* driver, RT_U8 address, RT_U8 endpoint, RT_U8* buffer, RT_U32 length);

#endif // USB_DRIVER_H

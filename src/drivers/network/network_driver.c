/**
 * @file network_driver.c
 * @brief Ağ Sürücüsü (Network Driver) Uygulaması
 * @version 1.0
 * @date 2025-03-15
 */

#include "network_driver.h"
#include "common/io_port.h"
#include "common/rt_types.h"
#include <string.h>

/* ================ LOCAL DEĞİŞKENLER ================ */

static RT_NetworkDriver network_driver_instance = {
    .base = {
        .id = 4,
        .name = "Network Driver",
        .version = "1.0",
        .state = DRIVER_STATE_UNINITIALIZED,
        .priority = RT_PRIORITY_HIGH,
        .irq_number = IRQ_NETWORK,
        .interrupt_handler = NULL
    },
    .num_devices = 0,
    .on_packet_received = NULL,
    .on_device_connected = NULL
};

/* ================ STATİK FONKSİYONLAR ================ */

// MAC adresi geçerlilik kontrolü
static RT_Bool Network_IsValidMAC(RT_U8* mac) {
    if (!mac) {
        return RT_FALSE;
    }
    for (RT_U8 i = 0; i < NET_MAC_ADDR_LEN; i++) {
        if (mac[i] != 0x00) {
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}

// IP adresi geçerlilik kontrolü
static RT_Bool Network_IsValidIP(RT_U8* ip) {
    if (!ip) {
        return RT_FALSE;
    }
    for (RT_U8 i = 0; i < NET_IP_ADDR_LEN; i++) {
        if (ip[i] != 0x00) {
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}

/* ================ GENEL FONKSİYONLAR ================ */

RT_ErrorCode Network_Init(RT_NetworkDriver* driver) {
    if (!driver) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    driver->base.state = DRIVER_STATE_READY;
    return RT_SUCCESS;
}

RT_ErrorCode Network_AddDevice(RT_NetworkDriver* driver, NetworkDevice* device) {
    if (!driver || !device || driver->num_devices >= NET_MAX_DEVICES) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // MAC ve IP adresi kontrolü
    if (!Network_IsValidMAC(device->mac_address) || !Network_IsValidIP(device->ip_address)) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Cihazı listeye ekle
    driver->devices[driver->num_devices++] = *device;

    // Callback çağır
    if (driver->on_device_connected) {
        driver->on_device_connected(device);
    }

    return RT_SUCCESS;
}

NetworkDevice* Network_FindDevice(RT_NetworkDriver* driver, RT_U8* mac_address) {
    if (!driver || !mac_address) {
        return NULL;
    }

    for (RT_U8 i = 0; i < driver->num_devices; i++) {
        if (memcmp(driver->devices[i].mac_address, mac_address, NET_MAC_ADDR_LEN) == 0) {
            return &driver->devices[i];
        }
    }
    return NULL;
}

RT_ErrorCode Network_SendPacket(RT_NetworkDriver* driver, RT_U8* destination_mac, EthernetPacket* packet) {
    if (!driver || !destination_mac || !packet || !Network_IsValidMAC(destination_mac)) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Paketin MAC adresini ayarla
    memcpy(packet->destination_mac, destination_mac, NET_MAC_ADDR_LEN);

    // Paketi gönder
    IO_Out32(0xE000, (RT_U32)packet); // Paket adresi
    IO_Out32(0xE004, packet->payload_length); // Paket uzunluğu
    IO_Out8(0xE008, 0x01); // Gönderme komutu

    return RT_SUCCESS;
}

RT_ErrorCode Network_ReceivePacket(RT_NetworkDriver* driver, EthernetPacket* packet) {
    if (!driver || !packet) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Paketi al
    packet->payload_length = IO_In32(0xE004); // Paket uzunluğu
    memcpy(packet, (void*)IO_In32(0xE000), sizeof(EthernetPacket)); // Paket verisi

    // Callback çağır
    if (driver->on_packet_received) {
        driver->on_packet_received(packet);
    }

    return RT_SUCCESS;
}

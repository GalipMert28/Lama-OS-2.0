/**
 * @file network_driver.h
 * @brief Ağ Sürücüsü (Network Driver) ve Protokolleri
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef NETWORK_DRIVER_H
#define NETWORK_DRIVER_H

#include "common/rt_drivers.h"
#include "common/rt_types.h"

/* ================ AĞ TANIMLAMALARI ================ */

#define NET_MAX_PACKET_SIZE        1514 // Maksimum paket boyutu (Ethernet)
#define NET_MAX_DEVICES            8    // Maksimum ağ cihazı sayısı
#define NET_IP_ADDR_LEN            4    // IPv4 adres uzunluğu
#define NET_MAC_ADDR_LEN           6    // MAC adres uzunluğu

/* ================ AĞ PROTOKOLLERİ ================ */

typedef enum {
    NET_PROTOCOL_ETHERNET = 0,    // Ethernet
    NET_PROTOCOL_IP,              // Internet Protocol (IP)
    NET_PROTOCOL_ARP,             // Address Resolution Protocol (ARP)
    NET_PROTOCOL_TCP,             // Transmission Control Protocol (TCP)
    NET_PROTOCOL_UDP              // User Datagram Protocol (UDP)
} NetworkProtocol;

/* ================ AĞ PAKET YAPISI ================ */

typedef struct {
    RT_U8  destination_mac[NET_MAC_ADDR_LEN]; // Hedef MAC adresi
    RT_U8  source_mac[NET_MAC_ADDR_LEN];      // Kaynak MAC adresi
    RT_U16 ether_type;                        // EtherType (Protokol tipi)
    RT_U8  payload[NET_MAX_PACKET_SIZE];      // Veri yükü
    RT_U16 payload_length;                    // Veri yükü uzunluğu
} EthernetPacket;

/* ================ AĞ CİHAZI YAPISI ================ */

typedef struct {
    RT_U8 mac_address[NET_MAC_ADDR_LEN];      // MAC adresi
    RT_U8 ip_address[NET_IP_ADDR_LEN];        // IP adresi
    NetworkProtocol supported_protocols[16];  // Desteklenen protokoller
    RT_U8 num_protocols;                      // Protokol sayısı
    RT_Bool is_active;                        // Cihaz aktif mi?
} NetworkDevice;

/* ================ AĞ SÜRÜCÜ YAPISI ================ */

typedef struct {
    RT_Driver base;                           // Temel sürücü yapısı
    NetworkDevice devices[NET_MAX_DEVICES];   // Ağ cihazları
    RT_U8 num_devices;                        // Bağlı cihaz sayısı
    void (*on_packet_received)(EthernetPacket*); // Paket alındığında çağrılacak fonksiyon
    void (*on_device_connected)(NetworkDevice*); // Cihaz bağlandığında çağrılacak fonksiyon
} RT_NetworkDriver;

/* ================ AĞ FONKSİYONLARI ================ */

// Ağ sürücüsü başlatma
RT_ErrorCode Network_Init(RT_NetworkDriver* driver);

// Ağ cihazı ekleme
RT_ErrorCode Network_AddDevice(RT_NetworkDriver* driver, NetworkDevice* device);

// Ağ cihazını bulma
NetworkDevice* Network_FindDevice(RT_NetworkDriver* driver, RT_U8* mac_address);

// Ağ paketi gönderme
RT_ErrorCode Network_SendPacket(RT_NetworkDriver* driver, RT_U8* destination_mac, EthernetPacket* packet);

// Ağ paketi alma
RT_ErrorCode Network_ReceivePacket(RT_NetworkDriver* driver, EthernetPacket* packet);

#endif // NETWORK_DRIVER_H

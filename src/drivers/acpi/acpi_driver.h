/**
 * @file acpi_driver.h
 * @brief ACPI (Advanced Configuration and Power Interface) Sürücüsü
 * @version 1.0
 * @date 2025-03-15
 */

#ifndef ACPI_DRIVER_H
#define ACPI_DRIVER_H

#include "common/rt_drivers.h"
#include "common/rt_types.h"

/* ================ ACPI TABLO ADRESLERİ ================ */

#define ACPI_RSDP_ADDRESS            0xE0000 // RSDP aranacak adres
#define ACPI_RSDP_SIGNATURE          "RSD PTR " // RSDP imzası

/* ================ ACPI TABLO TİPLERİ ================ */

typedef enum {
    ACPI_TABLE_RSDP = 0,          // Root System Description Pointer
    ACPI_TABLE_RSDT,              // Root System Description Table
    ACPI_TABLE_XSDT,              // Extended System Description Table
    ACPI_TABLE_FADT,              // Fixed ACPI Description Table
    ACPI_TABLE_DSDT,              // Differentiated System Description Table
    ACPI_TABLE_SSDT,              // Secondary System Description Table
    ACPI_TABLE_MADT,              // Multiple APIC Description Table
    ACPI_TABLE_MAX                // Maksimum tablo sayısı
} ACPI_TableType;

/* ================ RSDP YAPISI ================ */

typedef struct {
    RT_Char signature[8];         // "RSD PTR "
    RT_U8   checksum;             // Checksum
    RT_Char oem_id[6];            // OEM ID
    RT_U8   revision;             // Revision
    RT_U32  rsdt_address;         // RSDT fiziksel adresi
    // ACPI 2.0+ için genişletilmiş alanlar
    RT_U32  length;               // Tablo uzunluğu
    RT_U64  xsdt_address;         // XSDT fiziksel adresi
    RT_U8   extended_checksum;    // Genişletilmiş checksum
    RT_U8   reserved[3];          // Rezerve alan
} __attribute__((packed)) ACPI_RSDP;

/* ================ ACPI TABLO YAPISI ================ */

typedef struct {
    RT_Char signature[4];         // Tablo imzası (örneğin "RSDT")
    RT_U32  length;               // Tablo uzunluğu
    RT_U8   revision;             // Tablo revizyonu
    RT_U8   checksum;             // Checksum
    RT_Char oem_id[6];            // OEM ID
    RT_Char oem_table_id[8];      // OEM Tablo ID
    RT_U32  oem_revision;         // OEM Revizyon
    RT_U32  creator_id;           // Creator ID
    RT_U32  creator_revision;     // Creator Revizyon
} __attribute__((packed)) ACPI_TableHeader;

/* ================ SÜRÜCÜ YAPISI ================ */

typedef struct {
    RT_Driver         base;              // Temel sürücü yapısı
    ACPI_RSDP         rsdp;             // RSDP tablosu
    ACPI_TableHeader* tables[ACPI_TABLE_MAX]; // ACPI tabloları
    RT_Bool           acpi_2_0_supported; // ACPI 2.0+ desteği
    RT_U32            num_tables;       // Yüklenen tablo sayısı
    void (*on_power_event)(RT_U32);     // Güç olayı callback
} RT_ACPIDriver;

/* ================ FONKSİYONLAR ================ */

// ACPI sürücüsü başlatma
RT_ErrorCode ACPI_Init(RT_ACPIDriver* driver);

// ACPI tablolarını yükleme
RT_ErrorCode ACPI_LoadTables(RT_ACPIDriver* driver);

// ACPI tablosunu bulma
ACPI_TableHeader* ACPI_FindTable(RT_ACPIDriver* driver, const RT_Char* signature);

// Güç durumunu ayarlama
RT_ErrorCode ACPI_SetPowerState(RT_ACPIDriver* driver, RT_U8 state);

// CPU uyku durumuna alma
RT_ErrorCode ACPI_SuspendCPU(RT_ACPIDriver* driver);

#endif // ACPI_DRIVER_H

/**
 * @file acpi_driver.c
 * @brief ACPI (Advanced Configuration and Power Interface) Sürücüsü Uygulaması
 * @version 1.0
 * @date 2025-03-15
 */

#include "acpi_driver.h"
#include "common/io_port.h"
#include "common/rt_types.h"
#include <string.h>

/* ================ LOCAL DEĞİŞKENLER ================ */

static RT_ACPIDriver acpi_driver_instance = {
    .base = {
        .id = 2,
        .name = "ACPI Driver",
        .version = "1.0",
        .state = DRIVER_STATE_UNINITIALIZED,
        .priority = RT_PRIORITY_CRITICAL,
        .irq_number = IRQ_RTC,
        .interrupt_handler = NULL
    },
    .acpi_2_0_supported = RT_FALSE,
    .num_tables = 0,
    .on_power_event = NULL
};

/* ================ STATİK FONKSİYONLAR ================ */

// RSDP bulma
static RT_Bool ACPI_FindRSDP(void) {
    RT_U8* address = (RT_U8*)ACPI_RSDP_ADDRESS;
    ACPI_RSDP* rsdp;

    for (; address < (RT_U8*)(ACPI_RSDP_ADDRESS + 0xFFFFF); address += 16) {
        rsdp = (ACPI_RSDP*)address;

        // İmza kontrolü
        if (memcmp(rsdp->signature, ACPI_RSDP_SIGNATURE, 8) == 0) {
            // Checksum kontrolü
            RT_U8 sum = 0;
            for (RT_U32 i = 0; i < sizeof(ACPI_RSDP); i++) {
                sum += ((RT_U8*)rsdp)[i];
            }

            if (sum != 0) {
                continue;
            }

            // RSDP'yi kaydet
            memcpy(&acpi_driver_instance.rsdp, rsdp, sizeof(ACPI_RSDP));
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}

// ACPI tablosu checksum kontrolü
static RT_Bool ACPI_CheckTableChecksum(ACPI_TableHeader* table) {
    RT_U8 sum = 0;
    for (RT_U32 i = 0; i < table->length; i++) {
        sum += ((RT_U8*)table)[i];
    }
    return sum == 0;
}

/* ================ GENEL FONKSİYONLAR ================ */

RT_ErrorCode ACPI_Init(RT_ACPIDriver* driver) {
    if (!driver) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // RSDP bul
    if (!ACPI_FindRSDP()) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    // ACPI 2.0+ desteği kontrolü
    if (driver->rsdp.revision >= 2 && driver->rsdp.xsdt_address != 0) {
        driver->acpi_2_0_supported = RT_TRUE;
    }

    // Tabloları yükle
    RT_ErrorCode err = ACPI_LoadTables(driver);
    if (err != RT_SUCCESS) {
        return err;
    }

    driver->base.state = DRIVER_STATE_READY;
    return RT_SUCCESS;
}

RT_ErrorCode ACPI_LoadTables(RT_ACPIDriver* driver) {
    if (!driver || driver->base.state != DRIVER_STATE_INITIALIZING) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    ACPI_TableHeader* rsdt;
    RT_U32 num_entries;

    if (driver->acpi_2_0_supported) {
        rsdt = (ACPI_TableHeader*)driver->rsdp.xsdt_address;
    } else {
        rsdt = (ACPI_TableHeader*)driver->rsdp.rsdt_address;
    }

    // RSDT imzası kontrolü
    if (memcmp(rsdt->signature, "RSDT", 4) != 0) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    // Checksum kontrolü
    if (!ACPI_CheckTableChecksum(rsdt)) {
        return RT_ERROR_HARDWARE_FAULT;
    }

    // Tabloları yükle
    num_entries = (rsdt->length - sizeof(ACPI_TableHeader)) / (driver->acpi_2_0_supported ? 8 : 4);
    for (RT_U32 i = 0; i < num_entries; i++) {
        RT_U64 table_address;

        if (driver->acpi_2_0_supported) {
            table_address = ((RT_U64*)rsdt->length + i)[0];
        } else {
            table_address = ((RT_U32*)rsdt->length + i)[0];
        }

        ACPI_TableHeader* table = (ACPI_TableHeader*)table_address;

        // Geçersiz tablo kontrolü
        if (!ACPI_CheckTableChecksum(table)) {
            continue;
        }

        // Tabloyu kaydet
        driver->tables[driver->num_tables++] = table;
    }

    return RT_SUCCESS;
}

ACPI_TableHeader* ACPI_FindTable(RT_ACPIDriver* driver, const RT_Char* signature) {
    if (!driver || !signature || driver->base.state != DRIVER_STATE_READY) {
        return NULL;
    }

    for (RT_U32 i = 0; i < driver->num_tables; i++) {
        if (memcmp(driver->tables[i]->signature, signature, 4) == 0) {
            return driver->tables[i];
        }
    }

    return NULL;
}

RT_ErrorCode ACPI_SetPowerState(RT_ACPIDriver* driver, RT_U8 state) {
    if (!driver || driver->base.state != DRIVER_STATE_READY) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // Power state işlemleri burada yapılır
    return RT_SUCCESS;
}

RT_ErrorCode ACPI_SuspendCPU(RT_ACPIDriver* driver) {
    if (!driver || driver->base.state != DRIVER_STATE_READY) {
        return RT_ERROR_INVALID_PARAMETER;
    }

    // CPU uyku durumuna alınır
    __asm__ volatile("hlt");
    return RT_SUCCESS;
}

/**
 * @file io_port.c
 * @brief Donanım port I/O işlemleri
 * @version 1.0
 * @date 2025-03-15
 */

#include "io_port.h"

// Port konfigürasyonları için statik dizi
static IO_PortConfig port_configs[MAX_IO_PORTS] = {0};

// Port kilitleme için kullanılan dizi
static RT_Bool port_locks[MAX_IO_PORTS] = {RT_FALSE};

/* == STATİK FONKSİYONLAR == */

// Port numarası geçerlilik kontrolü
static RT_Bool IO_IsValidPort(RT_IOPort port) {
    return (port < MAX_IO_PORTS);
}

// Port kilidi kontrolü
static RT_Bool IO_IsPortLocked(RT_IOPort port) {
    if (!IO_IsValidPort(port)) {
        return RT_TRUE;
    }
    return port_locks[port];
}

/* == GENEL FONKSİYONLAR == */

RT_ErrorCode IO_InitPort(IO_PortConfig* config) {
    if (!config || !IO_IsValidPort(config->base)) {
        return IO_ERROR_INVALID_PORT;
    }

    // Port konfigürasyonunu kaydet
    port_configs[config->base] = *config;
    return RT_SUCCESS;
}

RT_ErrorCode IO_LockPort(RT_IOPort port) {
    if (!IO_IsValidPort(port)) {
        return IO_ERROR_INVALID_PORT;
    }

    if (port_locks[port]) {
        return IO_ERROR_PORT_LOCKED;
    }

    port_locks[port] = RT_TRUE;
    return RT_SUCCESS;
}

RT_ErrorCode IO_UnlockPort(RT_IOPort port) {
    if (!IO_IsValidPort(port)) {
        return IO_ERROR_INVALID_PORT;
    }

    port_locks[port] = RT_FALSE;
    return RT_SUCCESS;
}

RT_U8 IO_In8(RT_IOPort port) {
    if (!IO_IsValidPort(port) || IO_IsPortLocked(port)) {
        return 0;
    }

    RT_U8 value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void IO_Out8(RT_IOPort port, RT_U8 value) {
    if (!IO_IsValidPort(port) || IO_IsPortLocked(port)) {
        return;
    }

    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

RT_U16 IO_In16(RT_IOPort port) {
    if (!IO_IsValidPort(port) || IO_IsPortLocked(port)) {
        return 0;
    }

    RT_U16 value;
    __asm__ volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void IO_Out16(RT_IOPort port, RT_U16 value) {
    if (!IO_IsValidPort(port) || IO_IsPortLocked(port)) {
        return;
    }

    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

RT_U32 IO_In32(RT_IOPort port) {
    if (!IO_IsValidPort(port) || IO_IsPortLocked(port)) {
        return 0;
    }

    RT_U32 value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void IO_Out32(RT_IOPort port, RT_U32 value) {
    if (!IO_IsValidPort(port) || IO_IsPortLocked(port)) {
        return;
    }

    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

IO_PortConfig* IO_GetPortConfig(RT_IOPort port) {
    if (!IO_IsValidPort(port)) {
        return NULL;
    }
    return &port_configs[port];
}

RT_ErrorCode IO_SetPortMode(RT_IOPort port, IO_PortMode mode) {
    if (!IO_IsValidPort(port)) {
        return IO_ERROR_INVALID_PORT;
    }

    port_configs[port].mode = mode;
    return RT_SUCCESS;
}

RT_ErrorCode IO_SetPortAccessType(RT_IOPort port, IO_PortAccessType access_type) {
    if (!IO_IsValidPort(port)) {
        return IO_ERROR_INVALID_PORT;
    }

    port_configs[port].access_type = access_type;
    return RT_SUCCESS;
}

#include <windows.h>
#include "rpiq.h"
#include "mailbox.h"

ULONG GetBoardRevision() {
    ULONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_BOARD_REVISION mailbox;
    INIT_MAILBOX_GET_BOARD_REVISION(&mailbox);
    BOOL status = DeviceIoControl(hDevice, IOCTL_MAILBOX_PROPERTY,
        (LPVOID)(&mailbox), sizeof(mailbox),
        (LPVOID)(&mailbox), sizeof(mailbox), &bytesReturned, 0);
    if (status) value = mailbox.BoardRevision;

    CloseHandle(hDevice);
    return value;
}

ULONG GetVoltage(int voltageId) {
    ULONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_VOLTAGE mailbox;
    INIT_MAILBOX_GET_VOLTAGE(&mailbox, voltageId);
    BOOL status = DeviceIoControl(hDevice, IOCTL_MAILBOX_PROPERTY,
        (LPVOID)(&mailbox), sizeof(mailbox),
        (LPVOID)(&mailbox), sizeof(mailbox), &bytesReturned, 0);
    if (status) value = mailbox.Value;

    CloseHandle(hDevice);
    return value;
}

ULONG GetClock(int clockId) {
    ULONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_CLOCK_RATE mailbox;
    INIT_MAILBOX_GET_CLOCK_RATE(&mailbox, clockId);
    BOOL status = DeviceIoControl(hDevice, IOCTL_MAILBOX_PROPERTY,
        (LPVOID)(&mailbox), sizeof(mailbox),
        (LPVOID)(&mailbox), sizeof(mailbox), &bytesReturned, 0);
    if (status) value = mailbox.Rate;

    CloseHandle(hDevice);
    return value;
}

ULONG GetTemperature() {
    ULONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_TEMPERATURE mailbox;
    INIT_MAILBOX_GET_TEMPERATURE(&mailbox);
    BOOL status = DeviceIoControl(hDevice, IOCTL_MAILBOX_PROPERTY,
        (LPVOID)(&mailbox), sizeof(mailbox),
        (LPVOID)(&mailbox), sizeof(mailbox), &bytesReturned, 0);
    if (status) value = mailbox.Value;

    CloseHandle(hDevice);
    return value;
}

ULONG GetFirmwareRevision() {
    ULONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_FIRMWARE_REVISION mailbox;
    INIT_MAILBOX_GET_FIRMWARE_REVISION(&mailbox);
    BOOL status = DeviceIoControl(hDevice, IOCTL_MAILBOX_PROPERTY,
        (LPVOID)(&mailbox), sizeof(mailbox),
        (LPVOID)(&mailbox), sizeof(mailbox), &bytesReturned, 0);
    if (status) value = mailbox.FirmwareRevision;

    CloseHandle(hDevice);
    return value;
}

ULONGLONG GetSerialNumber() {
    ULONGLONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_BOARD_SERIAL mailbox;
    INIT_MAILBOX_GET_BOARD_SERIAL(&mailbox);
    BOOL status = DeviceIoControl(hDevice, IOCTL_MAILBOX_PROPERTY,
        (LPVOID)(&mailbox), sizeof(mailbox),
        (LPVOID)(&mailbox), sizeof(mailbox), &bytesReturned, 0);
    if (status) {
        value = *(ULONGLONG*)(mailbox.BoardSerial);
    }

    CloseHandle(hDevice);
    return value;
}

ULONG GetInstalledMemory(ULONG BoardRevision) {
    //
    // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
    // Bits [20-22] indicate the amount of memory starting with 256MB (000b)
    // and doubling in size for each value (001b = 512 MB, 010b = 1GB, etc.)
    //
    return 256 << ((BoardRevision >> 20) & 0x07);
}

const wchar_t* GetProcessorName(ULONG BoardRevision) {
    ULONG cpuId = (BoardRevision >> 12) & 0x0F;

    switch (cpuId) {
        // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
    case 0x00:
        return L"BCM2835 (ARM11)";
    case 0x01:
        return L"BCM2836 (ARM Cortex-A7)";
    case 0x02:
        return L"BCM2837 (ARM Cortex-A53)";
    case 0x03:
        return L"BCM2711 (ARM Cortex-A72)";
    default:
        return L"Unknown";
    }
}

const wchar_t* GetPiModelName(ULONG BoardRevision) {
    ULONG modelId = (BoardRevision >> 4) & 0xFF;

    // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
    switch (modelId) {
    case 0x00:
        return L"Raspberry Pi Model A";
    case 0x01:
        return L"Raspberry Pi Model B";
    case 0x02:
        return L"Raspberry Pi Model A+";
    case 0x03:
        return L"Raspberry Pi Model B+";
    case 0x04:
        return L"Raspberry Pi 2 Model B";
    case 0x06:
        return L"Raspberry Pi Compute Module 1";
    case 0x08:
        return L"Raspberry Pi 3 Model B";
    case 0x09:
        return L"Raspberry Pi Zero";
    case 0x0A:
        return L"Raspberry Pi Compute Module 3";
    case 0x0C:
        return L"Raspberry Pi Zero W";
    case 0x0D:
        return L"Raspberry Pi 3 Model B+";
    case 0x0E:
        return L"Raspberry Pi 3 Model A+";
    case 0x10:
        return L"Raspberry Pi Compute Module 3+";
    case 0x11:
        return L"Raspberry Pi 4 Model B";
    default:
        return L"Unknown";
    }
}
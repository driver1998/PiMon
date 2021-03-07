#include <windows.h>
#include <strsafe.h>
#include <pdh.h>
#include "rpiq.h"
#include "utils.h"
#include "data.h"

#ifdef _DEBUG
#define DEBUGLOG(x, ...) wprintf(x, __VA_ARGS__)
#else
#define DEBUGLOG(x, ...)
#endif

static const wchar_t* RpiProcessorName[] = {
    [CPU_UNKNOWN] = L"Unknown",
    [CPU_BCM2835] = L"BCM2835 (ARM11)",
    [CPU_BCM2836] = L"BCM2836 (ARM Cortex-A7)",
    [CPU_BCM2837] = L"BCM2837 (ARM Cortex-A53)",
    [CPU_BCM2711] = L"BCM2711 (ARM Cortex-A72)",
};

static const wchar_t* RpiModelName[] = {
    [MODEL_A]        = L"Raspberry Pi Model A",
    [MODEL_B]        = L"Raspberry Pi Model B",
    [MODEL_A_PLUS]   = L"Raspberry Pi Model A+",
    [MODEL_B_PLUS]   = L"Raspberry Pi Model B+",
    [MODEL_2B]       = L"Raspberry Pi 2 Model B",
    [MODEL_CM1]      = L"Raspberry Pi Compute Module 1",
    [MODEL_3B]       = L"Raspberry Pi 3 Model B",
    [MODEL_ZERO]     = L"Raspberry Pi Zero",
    [MODEL_CM3]      = L"Raspberry Pi Compute Module 3",
    [MODEL_ZERO_W]   = L"Raspberry Pi Zero W",
    [MODEL_3B_PLUS]  = L"Raspberry Pi 3 Model B+",
    [MODEL_3A_PLUS]  = L"Raspberry Pi 3 Model A+",
    [MODEL_CM3_PLUS] = L"Raspberry Pi Compute Module 3+",
    [MODEL_4B]       = L"Raspberry Pi 4 Model B",
    [MODEL_PI400]    = L"Raspberry Pi 400",
    [MODEL_UNKNOWN]  = L"Unknown Pi Model"
};

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

ULONG GetMeasuredClock(int clockId) {
    ULONG value = 0;

    HANDLE hDevice = CreateFileW(
        RPIQ_USERMODE_PATH, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0
    );
    if (hDevice == INVALID_HANDLE_VALUE) return value;

    DWORD bytesReturned;

    MAILBOX_GET_MEASURED_CLOCK_RATE mailbox;
    INIT_MAILBOX_GET_MEASURED_CLOCK_RATE(&mailbox, clockId);
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

ULONG GetWindowsMemory() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    return (ULONG)(memStatus.ullTotalPhys / 1048576ULL);
}

ULONG GetInstalledMemory(ULONG BoardRevision) {
    //
    // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
    // Bits [20-22] indicate the amount of memory starting with 256MB (000b)
    // and doubling in size for each value (001b = 512 MB, 010b = 1GB, etc.)
    //
    return 256 << ((BoardRevision >> 20) & 0x07);
}

enum RpiProcessor GetProcessorType(ULONG BoardRevision)
{
    // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
    ULONG cpuId = (BoardRevision >> 12) & 0x0F;

    switch (cpuId) {
    case 0x00:
        return CPU_BCM2835;
    case 0x01:
        return CPU_BCM2836;
    case 0x02:
        return CPU_BCM2837;
    case 0x03:
        return CPU_BCM2711;
    default:
        return CPU_UNKNOWN;
    }
}

enum RpiModel GetPiModel(ULONG BoardRevision)
{
    // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
    ULONG modelId = (BoardRevision >> 4) & 0xFF;

    switch (modelId) {
    case 0x00:
        return MODEL_A;
    case 0x01:
        return MODEL_B;
    case 0x02:
        return MODEL_A_PLUS;
    case 0x03:
        return MODEL_B_PLUS;
    case 0x04:
        return MODEL_2B;
    case 0x06:
        return MODEL_CM1;
    case 0x08:
        return MODEL_3B;
    case 0x09:
        return MODEL_ZERO;
    case 0x0A:
        return MODEL_CM3;
    case 0x0C:
        return MODEL_ZERO_W;
    case 0x0D:
        return MODEL_3B_PLUS;
    case 0x0E:
        return MODEL_3A_PLUS;
    case 0x10:
        return MODEL_CM3_PLUS;
    case 0x11:
        return MODEL_4B;
    case 0x13:
        return MODEL_PI400;
    default:
        return MODEL_UNKNOWN;
    }
}

const wchar_t* GetProcessorName(ULONG BoardRevision)
{
    return RpiProcessorName[GetProcessorType(BoardRevision)];
}

const wchar_t* GetPiModelName(ULONG BoardRevision)
{
    return RpiModelName[GetPiModel(BoardRevision)];
}

wchar_t* GetBiosVersion() {
    return RegQueryString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BIOSVersion");
}

wchar_t* GetWindowsVersion() {
    DWORD ubr = RegQueryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"UBR");
    wchar_t* currentBuild = RegQueryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuild");
    wchar_t* productName = RegQueryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName");

    // PROCESSOR_ARCHITEW6432 is the native arch when running in WOW64
    wchar_t arch[10];
    if (!GetEnvironmentVariableW(L"PROCESSOR_ARCHITEW6432", arch, 10))
        GetEnvironmentVariableW(L"PROCESSOR_ARCHITECTURE", arch, 10);

    wchar_t* str = (wchar_t*)malloc(100 * sizeof(wchar_t));
    if (str != NULL) StringCchPrintfW(str, 100, L"%ws %ws Build %ws.%d", productName, arch, currentBuild, ubr);

    free(currentBuild);
    free(productName);
    return str;
}


ULONG GetTemperatureAcpi() {
    LONGLONG value = 0;
    ULONG status;

    DWORD queryId = 0;
    PDH_HQUERY hQuery = NULL;
    status = PdhOpenQueryW(NULL, (DWORD_PTR)&queryId, &hQuery);
    if (status != ERROR_SUCCESS) {
        DEBUGLOG(L"PdhOpenQuery() Failed, status=%08x", status);
        goto err0;
    }

    DWORD size = 0;
    wchar_t* pathList = NULL;
    const wchar_t* wildcardPath = L"\\Thermal Zone Information(*)\\High Precision Temperature";
    PdhExpandWildCardPathW(NULL, wildcardPath, pathList, &size, 0);
    pathList = malloc(sizeof(wchar_t) * size);
    if (pathList == NULL) goto err1;

    PdhExpandWildCardPathW(NULL, wildcardPath, pathList, &size, 0);
    if (status != ERROR_SUCCESS) {
        DEBUGLOG(L"PdhExpandWildCardPath() Failed, status=%08x", status);
        goto err2;
    }

    // Raspberry Pi only have one thermal zone
    DWORD counterId = 0;
    PDH_HCOUNTER hCounter = NULL;
    const wchar_t* path = pathList;
    status = PdhAddCounterW(hQuery, path, (DWORD_PTR)&counterId, &hCounter);
    if (status != ERROR_SUCCESS) {
        DEBUGLOG(L"PdhAddCounter() Failed, status=%08x", status);
        goto err2;
    }

    status = PdhCollectQueryData(hQuery);
    if (status != ERROR_SUCCESS) {
        DEBUGLOG(L"PdhCollectQueryData() Failed, status=%08x", status);
        goto err2;
    }

    PDH_RAW_COUNTER raw = { 0 };
    DWORD type = PERF_COUNTER_RAWCOUNT;
    status = PdhGetRawCounterValue(hCounter, NULL, &raw);
    if (status != ERROR_SUCCESS) {
        DEBUGLOG(L"PdhGetRawCounterValue() Failed, status=%08x", status);
        goto err2;
    }
    value = raw.FirstValue;

err2:
    if (pathList != NULL) free(pathList);
err1:
    if (hQuery != NULL) PdhCloseQuery(hQuery);
err0:
    return (ULONG)value;
}
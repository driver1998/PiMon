#ifndef MAILBOX_H
#define MAILBOX_H

#include <windows.h>

enum RpiProcessor {
	CPU_UNKNOWN = 0,
	CPU_BCM2835 = 1,
	CPU_BCM2836 = 2,
	CPU_BCM2837 = 3,
	CPU_BCM2711 = 4
};

enum RpiModel {
	MODEL_UNKNOWN  = 0,
	MODEL_A        = 1,
	MODEL_B        = 2,
	MODEL_A_PLUS   = 3,
	MODEL_B_PLUS   = 4,
	MODEL_2B       = 5,
	MODEL_CM1      = 6,
	MODEL_3B       = 7,
	MODEL_ZERO     = 8,
	MODEL_CM3      = 9,
	MODEL_ZERO_W   = 10,
	MODEL_3B_PLUS  = 11,
	MODEL_3A_PLUS  = 12,
	MODEL_CM3_PLUS = 13,
	MODEL_4B       = 14,
};

ULONG GetBoardRevision();
ULONG GetVoltage(int voltageId);
ULONG GetClock(int clockId);
ULONG GetMeasuredClock(int clockId);
ULONG GetTemperature();
ULONG GetTemperatureAcpi();
ULONG GetFirmwareRevision();
ULONGLONG GetSerialNumber();
ULONG GetWindowsMemory();
ULONG GetInstalledMemory(ULONG BoardRevision);
enum RpiProcessor GetProcessorType(ULONG BoardRevision);
enum RpiModel GetPiModel(ULONG BoardRevision);
const wchar_t* GetProcessorName(ULONG BoardRevision);
const wchar_t* GetPiModelName(ULONG BoardRevision);

// Free the string after use
wchar_t* GetBiosVersion();
wchar_t* GetWindowsVersion();

#endif
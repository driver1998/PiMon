#ifndef MAILBOX_H
#define MAILBOX_H

#include <windows.h>

ULONG GetBoardRevision();
ULONG GetVoltage(int voltageId);
ULONG GetClock(int clockId);
ULONG GetTemperature();
ULONG GetFirmwareRevision();
ULONGLONG GetSerialNumber();
ULONG GetInstalledMemory(ULONG BoardRevision);
const wchar_t* GetProcessorName(ULONG BoardRevision);
const wchar_t* GetPiModelName(ULONG BoardRevision);

#endif
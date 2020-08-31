#ifndef REG_H
#define REG_H

#include <windows.h>
#include <strsafe.h>

ULONG GetWindowsMemory();

DWORD RegQueryDword(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName);

// Free the string after use
wchar_t* RegQueryString(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName);

// Free the string after use
wchar_t* GetBiosVersion();

// Free the string after use
wchar_t* GetWindowsVersion();

#endif
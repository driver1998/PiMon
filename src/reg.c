#include <windows.h>
#include "reg.h"

ULONG GetWindowsMemory() {
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memStatus);

	return (ULONG)(memStatus.ullTotalPhys / 1048576ULL);
}

DWORD RegQueryDword(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName) {
	HKEY hSubKey;
	DWORD type = REG_DWORD;

	LSTATUS status = RegOpenKeyExW(hKey, subKey, 0, KEY_READ | KEY_WOW64_64KEY, &hSubKey);
	if (status != ERROR_SUCCESS) return 0;

	DWORD value = 0;
	DWORD size = sizeof(DWORD);

	status = RegQueryValueExW(hSubKey, valueName, NULL, &type, (LPBYTE)&value, &size);
	if (status != ERROR_SUCCESS) value = 0;

	RegCloseKey(hSubKey);
	return value;
}

wchar_t* RegQueryString(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName) {
	HKEY hSubKey;
	DWORD size = 0;
	DWORD type = REG_SZ;

	LSTATUS status = RegOpenKeyExW(hKey, subKey, 0, KEY_READ | KEY_WOW64_64KEY, &hSubKey);
	if (status != ERROR_SUCCESS) return NULL;

	RegQueryValueExW(hSubKey, valueName, NULL, &type, NULL, &size);
	wchar_t* str = (wchar_t*)malloc(size);

	status = RegQueryValueExW(hSubKey, valueName, NULL, &type, (LPBYTE)str, &size);
	if (status != ERROR_SUCCESS) {
		free(str);
		str = NULL;
	}

	RegCloseKey(hSubKey);
	return str;
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
	if (str != NULL) StringCbPrintfW(str, 100, L"%ws %ws Build %ws.%d", productName, arch, currentBuild, ubr);

	free(currentBuild);
	free(productName);
	return str;
}

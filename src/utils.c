#include <windows.h>

#include <strsafe.h>
#include <PathCch.h>
#include "utils.h"
#include "version.h"

void ResizeWindowByClientArea(HWND hwnd, int width, int height) {
	UINT dpi = GetDpiForWindow(hwnd);
	int w = SCALE(width, dpi) +
		(GetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	int h = SCALE(height, dpi) +
		(GetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2 +
		GetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE);
}

void MoveWindowToCenterOfScreen(HWND hwnd) {
	RECT rs = { 0 };
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &rs, 0);
	RECT r = { 0 };
	GetWindowRect(hwnd, &r);

	int x = (rs.right - (r.right - r.left)) / 2;
	int y = (rs.bottom - (r.bottom - r.top)) / 2;

	SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE);
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

int PrettyPrintUnits(double* value, const wchar_t** units, int unit_count, double unit_step) {
	int unit_index = 0;
	while (*value >= unit_step && unit_index < (unit_count - 1)) {
		*value /= unit_step;
		unit_index++;
	}
	return unit_index;
}

#include <windows.h>
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

int PrettyPrintUnits(double* value, const wchar_t** units, int unit_count, double unit_step) {
	int unit_index = 0;
	while (*value >= unit_step && unit_index < (unit_count - 1)) {
		*value /= unit_step;
		unit_index++;
	}
	return unit_index;
}

int ConfigGetInt(const wchar_t* section, const wchar_t* key, int default_value) {
	wchar_t exeDir[MAX_PATH] = { 0 };
	wchar_t configPath[MAX_PATH] = { 0 };
	GetExecutableDirectory(exeDir, MAX_PATH);
	PathCchCombine(configPath, MAX_PATH, exeDir, PIMON_APPNAME L".ini");
	return GetPrivateProfileIntW(section, key, default_value, configPath);
}

BOOL ConfigSetInt(const wchar_t* section, const wchar_t* key, int value) {
	wchar_t exeDir[MAX_PATH] = { 0 };
	wchar_t configPath[MAX_PATH] = { 0 };
	GetExecutableDirectory(exeDir, MAX_PATH);
	PathCchCombine(configPath, MAX_PATH, exeDir, PIMON_APPNAME L".ini");
	return WritePrivateProfileStringW(section, key, value ? L"1" : L"0", configPath);
}

void GetExecutableDirectory(wchar_t* path, DWORD size) {
	if (!path) return;
	DWORD length = GetModuleFileNameW(NULL, path, size);
	PathCchRemoveFileSpec(path, size);
}
#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <CommCtrl.h>

#define SCALE(x, dpi) ((int)((x) * (dpi / 96.0)))

void LVAddColumn(HWND hwnd, int pos, const wchar_t* header, int width) {
	size_t len = wcslen(header) + 1;
	wchar_t* str = (wchar_t*)malloc(len * sizeof(wchar_t));
	if (str == NULL) return;
	wcscpy_s(str, len, header);

	LVCOLUMNW col = { 0 };
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.pszText = str;
	col.cchTextMax = (int)wcslen(str);
	col.cx = width;
	ListView_InsertColumn(hwnd, pos, &col);

	free(str);
}

void LVSetColumnWidth(HWND hwnd, int iCol, int width) {
	LVCOLUMNW col = { 0 };
	col.mask = LVCF_WIDTH;
	col.cx = width;
	ListView_SetColumn(hwnd, iCol, &col);
}

void LVSetItemText(HWND hwnd, int iItem, int iSubItem, const wchar_t* text) {
	size_t len = wcslen(text) + 1;
	wchar_t* str = (wchar_t*)malloc(len * sizeof(wchar_t));
	if (str == NULL) return;
	wcscpy_s(str, len, text);

	LVITEMW item = { 0 };
	item.mask = LVIF_TEXT;
	item.iItem = iItem;
	item.iSubItem = iSubItem;
	item.pszText = str;
	item.cchTextMax = (int)wcslen(str);
	ListView_SetItem(hwnd, &item);

	free(str);
}

void LVSetItemGroupId(HWND hwnd, int iItem, int groupId) {
	LVITEMW item = { 0 };
	item.mask = LVIF_GROUPID;
	item.iItem = iItem;
	item.iGroupId = groupId;
	ListView_SetItem(hwnd, &item);
}

void LVSetItemIndent(HWND hwnd, int iItem, int indent) {
	LVITEMW item = { 0 };
	item.mask = LVIF_INDENT;
	item.iItem = iItem;
	item.iIndent = indent;
	ListView_SetItem(hwnd, &item);
}


void LVSetItemImage(HWND hwnd, int iItem, int imageId) {
	LVITEMW item = { 0 };
	item.mask = LVIF_IMAGE;
	item.iItem = iItem;
	item.iImage = imageId;
	ListView_SetItem(hwnd, &item);
}


void LVAddGroup(HWND hwnd, int pos, const wchar_t* header, int id) {
	size_t len = wcslen(header) + 1;
	wchar_t* str = (wchar_t*)malloc(len * sizeof(wchar_t));
	if (str == NULL) return;
	wcscpy_s(str, len, header);

	LVGROUP group = { 0 };
	group.cbSize = sizeof(LVGROUP);
	group.mask = LVGF_HEADER | LVGF_GROUPID;
	group.pszHeader = str;
	group.cchHeader = (int)wcslen(str);
	group.iGroupId = id;
	ListView_InsertGroup(hwnd, pos, &group);

	free(str);
}

int LVAddItem(HWND hwnd, int pos, const wchar_t* text) {
	int index = 0;
	if (pos < 0) pos = ListView_GetItemCount(hwnd);
	
	size_t len = wcslen(text) + 1;
	wchar_t* str = (wchar_t*)malloc(len * sizeof(wchar_t));
	if (str == NULL) return index;
	wcscpy_s(str, len, text);

	LVITEM item = { 0 };
	item.mask = LVIF_TEXT;
	item.iItem = pos;
	item.pszText = str;
	item.cchTextMax = (int)wcslen(str);
	index = ListView_InsertItem(hwnd, &item);

	free(str);
	return index;
}

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

// Free the string after use
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

// Free the string after use
wchar_t* GetBiosVersion() {
	return RegQueryString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BIOSVersion");
}

// Free the string after use
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

#endif
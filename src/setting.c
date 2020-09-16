#include <windows.h>
#include <PathCch.h>
#include <strsafe.h>
#include <CommCtrl.h>
#include <windowsx.h>
#include <ShlObj.h>
#include "setting.h"
#include "resource.h"
#include "data.h"
#include "utils.h"

// Global config
BOOL alwaysOnTop;
BOOL hideOnMinimize;
BOOL acpiThermal;
COLORREF trayBackground;
COLORREF trayForeground;

INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static COLORREF custom_colors[16] = { 0 };
	static COLORREF tmpForeground;
	static COLORREF tmpBackground;
	static BOOL tmpAcpiThermal;

	static HWND hwndPreview;
	static HWND hwndAcpiThermal;
	static HWND hwndOK;
	static HBRUSH bkgBrush = NULL;

	CHOOSECOLORW c = { 0 };
	c.lStructSize = sizeof(CHOOSECOLORW);
	c.hwndOwner = hwnd;
	c.lpCustColors = custom_colors;
	c.Flags = CC_RGBINIT;

	switch (uMsg) {
	case WM_INITDIALOG:
		MoveWindowToCenterOfScreen(hwnd);
		hwndPreview = GetDlgItem(hwnd, IDC_PREVIEW);
		hwndAcpiThermal = GetDlgItem(hwnd, IDC_ACPI_THERMAL);
		hwndOK = GetDlgItem(hwnd, IDOK);

		tmpForeground = trayForeground;
		tmpBackground = trayBackground;
		tmpAcpiThermal = acpiThermal;
		bkgBrush = CreateSolidBrush(tmpBackground);

		Button_SetCheck(hwndAcpiThermal, tmpAcpiThermal ? BST_CHECKED : BST_UNCHECKED);

		if (GetProcessorType(GetBoardRevision()) != CPU_BCM2711)
			Button_Enable(hwndAcpiThermal, FALSE);

		return TRUE;
	case WM_CTLCOLORSTATIC:
		if ((HWND)lParam == hwndPreview) {
			SetTextColor((HDC)wParam, tmpForeground);
			SetBkColor((HDC)wParam, tmpBackground);
			return (INT_PTR)bkgBrush;
		} else {
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (INT_PTR)GetStockObject(NULL_BRUSH);
		}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FOREGROUND:
			c.rgbResult = tmpForeground;
			ChooseColorW(&c);
			tmpForeground = c.rgbResult;

			InvalidateRect(hwndPreview, NULL, TRUE);
			return TRUE;
		case IDC_BACKGROUND:
			c.rgbResult = tmpBackground;
			ChooseColorW(&c);
			tmpBackground = c.rgbResult;

			DeleteObject(bkgBrush);
			bkgBrush = CreateSolidBrush(tmpBackground);
			InvalidateRect(hwndPreview, NULL, TRUE);

			return TRUE;
		case IDC_ACPI_THERMAL:
			tmpAcpiThermal = (Button_GetCheck(hwndAcpiThermal) == BST_CHECKED);
			Button_SetElevationRequiredState(hwndOK, tmpAcpiThermal && !acpiThermal && !IsUserAnAdmin());
			return TRUE;
		case IDOK:
			trayForeground = tmpForeground;
			ConfigSetColor(PIMON_APPNAME, CONFIG_TRAY_FOREGROUND, trayForeground);

			trayBackground = tmpBackground;
			ConfigSetColor(PIMON_APPNAME, CONFIG_TRAY_BACKGROUND, trayBackground);

			acpiThermal = tmpAcpiThermal;
			ConfigSetInt(PIMON_APPNAME, CONFIG_ACPI_THERMAL_ZONE, acpiThermal);
		case IDCANCEL:
			EndDialog(hwnd, wParam);
			return TRUE;
		}
	}
	return FALSE;
}


void LoadConfig() {
	alwaysOnTop = ConfigGetInt(PIMON_APPNAME, CONFIG_ALWAYS_ON_TOP, FALSE);
	hideOnMinimize = ConfigGetInt(PIMON_APPNAME, CONFIG_HIDE_ON_MINIMIZE, FALSE);
	acpiThermal = ConfigGetInt(PIMON_APPNAME, CONFIG_ACPI_THERMAL_ZONE, FALSE);
	trayBackground = ConfigGetColor(PIMON_APPNAME, CONFIG_TRAY_BACKGROUND, RGB(128, 0, 0));
	trayForeground = ConfigGetColor(PIMON_APPNAME, CONFIG_TRAY_FOREGROUND, RGB(255, 255, 255));
}


void GetExecutableDirectory(wchar_t* path, DWORD size) {
	if (!path) return;
	GetModuleFileNameW(NULL, path, size);
	PathCchRemoveFileSpec(path, size);
}

void GetConfigFile(wchar_t* path, DWORD size) {
	if (!path) return;
	wchar_t exeDir[MAX_PATH] = { 0 };
	GetExecutableDirectory(exeDir, MAX_PATH);
	PathCchCombine(path, size, exeDir, PIMON_APPNAME L".ini");
}

int ConfigGetInt(const wchar_t* section, const wchar_t* key, int default_value) {
	wchar_t configPath[MAX_PATH] = { 0 };
	GetConfigFile(configPath, MAX_PATH);
	return GetPrivateProfileIntW(section, key, default_value, configPath);
}

BOOL ConfigSetInt(const wchar_t* section, const wchar_t* key, int value) {
	wchar_t configPath[MAX_PATH] = { 0 };
	GetConfigFile(configPath, MAX_PATH);
	return WritePrivateProfileStringW(section, key, value ? L"1" : L"0", configPath);
}

COLORREF ParseColorString(const wchar_t* str) {
	ULONG r, g, b;
	if (swscanf_s(str, L"#%02X%02x%02x", &r, &g, &b) < 3) return -1;
	if (r > 255 || g > 255 || b > 255) return -1;
	return RGB(r, g, b);
}

void GetColorString(COLORREF color, wchar_t* str, size_t size) {
	if (str == NULL || size < 8 * sizeof(wchar_t)) return;
	BYTE r = GetRValue(color);
	BYTE g = GetGValue(color);
	BYTE b = GetBValue(color);
	StringCbPrintfW(str, size, L"#%02X%02X%02X", r, g, b);
}

COLORREF ConfigGetColor(const wchar_t* section, const wchar_t* key, COLORREF default_value) {
	wchar_t configPath[MAX_PATH] = { 0 };
	GetConfigFile(configPath, MAX_PATH);

	wchar_t def[10] = { 0 };
	GetColorString(default_value, def, sizeof(def));

	wchar_t str[10] = { 0 };
	GetPrivateProfileStringW(section, key, def, str, 10, configPath);

	COLORREF c = ParseColorString(str);
	if (c > RGB(255, 255, 255))
		return default_value;
	else
		return c;
}

void ConfigSetColor(const wchar_t* section, const wchar_t* key, COLORREF value) {
	wchar_t configPath[MAX_PATH] = { 0 };
	GetConfigFile(configPath, MAX_PATH);

	wchar_t str[10] = { 0 };
	GetColorString(value, str, sizeof(str));

	WritePrivateProfileStringW(section, key, str, configPath);
}

#ifndef SETTING_H
#define SETTING_H

#include <windows.h>

extern BOOL alwaysOnTop;
extern BOOL hideOnMinimize;
extern BOOL acpiThermal;
extern COLORREF trayBackground;
extern COLORREF trayForeground;

#define CONFIG_ALWAYS_ON_TOP     L"AlwaysOnTop"
#define CONFIG_HIDE_ON_MINIMIZE  L"HideOnMinimize"
#define CONFIG_ACPI_THERMAL_ZONE L"UseAcpiThermalZone"
#define CONFIG_TRAY_BACKGROUND   L"TrayIconBackground"
#define CONFIG_TRAY_FOREGROUND   L"TrayIconForeground"

INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void LoadConfig();
int      ConfigGetInt(const wchar_t* section, const wchar_t* key, int default_value);
BOOL     ConfigSetInt(const wchar_t* section, const wchar_t* key, int value);
COLORREF ConfigGetColor(const wchar_t* section, const wchar_t* key, COLORREF default_value);
void     ConfigSetColor(const wchar_t* section, const wchar_t* key, COLORREF value);

#endif
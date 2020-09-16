#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

#define SCALE(x, dpi) ((int)((x) * (dpi / 96.0)))
void ResizeWindowByClientArea(HWND hwnd, int width, int height);
void MoveWindowToCenterOfScreen(HWND hwnd);

// Free the string after use
wchar_t* RegQueryString(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName);
DWORD RegQueryDword(HKEY hKey, LPCWSTR subKey, LPCWSTR valueName);

int PrettyPrintUnits(double* value, const wchar_t** units, int unit_count, double unit_step);

#endif
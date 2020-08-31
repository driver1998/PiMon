#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

#define SCALE(x, dpi) ((int)((x) * (dpi / 96.0)))
void ResizeWindowByClientArea(HWND hwnd, int width, int height);
void MoveWindowToCenterOfScreen(HWND hwnd);
void GetExecutableDirectory(wchar_t* path, DWORD size);
int ConfigGetInt(const wchar_t* section, const wchar_t* key, int default_value);
BOOL ConfigSetInt(const wchar_t* section, const wchar_t* key, int value);
int PrettyPrintUnits(double* value, const wchar_t** units, int unit_count, double unit_step);

#endif
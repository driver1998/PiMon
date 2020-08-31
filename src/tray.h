#ifndef TRAY_H
#define TRAY_H

#include <windows.h>

void AddTrayIcon(HWND hwnd, UINT id, const wchar_t* desc, HICON icon, UINT msgId);
void DeleteTrayIcon(HWND hwnd, UINT id);
void UpdateTrayIcon(HWND hwnd, UINT id, HICON icon, const wchar_t* desc);

// Remember to destroy the icon
HICON DrawTrayIcon(HWND hwnd, COLORREF bkColor, COLORREF textColor, double value);

#endif

#include <windows.h>
#include <strsafe.h>
#include "tray.h"
#include "utils.h"

void AddTrayIcon(HWND hwnd, UINT id, const wchar_t* desc, HICON icon, UINT msgId) {
	NOTIFYICONDATAW nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hwnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = icon;
	nid.uCallbackMessage = msgId;
	nid.uID = id;
	wcscpy_s(nid.szTip, 128, desc);
	Shell_NotifyIconW(NIM_ADD, &nid);
}

void DeleteTrayIcon(HWND hwnd, UINT id) {
	NOTIFYICONDATAW nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hwnd;
	nid.uFlags = 0;
	nid.uID = id;
	Shell_NotifyIconW(NIM_DELETE, &nid);
}

HICON DrawTrayIcon(HWND hwnd, COLORREF bkColor, COLORREF textColor, double value) {
	int dpi = GetDpiForWindow(hwnd);
	int size = SCALE(16, dpi);

	// Create Memory Bitmap
	HDC dc = GetDC(hwnd);
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP hcolor = CreateCompatibleBitmap(dc, size, size);
	SelectObject(memDC, hcolor);

	// Fill Background
	RECT r = { 0, 0, size, size };
	HBRUSH brush = CreateSolidBrush(bkColor);
	FillRect(memDC, &r, brush);
	SetBkColor(memDC, bkColor);

	// Set Text Color
	SetTextColor(memDC, textColor);

	// Set Font
	LOGFONTW font = { 0 };
	font.lfWeight = FW_NORMAL;
	font.lfPitchAndFamily = DEFAULT_PITCH;
	if (value < 100.0)
		font.lfHeight = -9 * dpi / 72;
	else
		font.lfHeight = -8 * dpi / 72;
	wcscpy_s(font.lfFaceName, size, L"Segoe UI");
	HFONT hfont = CreateFontIndirectW(&font);
	SelectObject(memDC, hfont);

	// Determine a sane format
	wchar_t str[5];
	const wchar_t* format;
	if (value < 1.0)
		format = L"%.1g";
	else if (value < 100.0)
		format = L"%.2g";
	else
		format = L"%.3g";
	StringCbPrintfW(str, sizeof(str), format, value);

	// Draw text
	DrawTextW(memDC, str, wcslen(str), &r, DT_SINGLELINE | DT_CALCRECT);
	r.left = (size - r.right) / 2; r.right += r.left;
	r.top = (size - r.bottom) / 2; r.bottom += r.top;
	DrawTextW(memDC, str, wcslen(str), &r, DT_SINGLELINE);

	// Create Icon
	HBITMAP hmask = CreateBitmap(size, size, 1, 1, NULL);
	ICONINFO ii = { TRUE, 0, 0, hmask, hcolor };
	HICON trayIcon = CreateIconIndirect(&ii);

	// Cleanup
	DeleteObject(brush);
	DeleteObject(hcolor);
	DeleteObject(hmask);
	DeleteObject(hfont);
	DeleteDC(memDC);
	ReleaseDC(hwnd, dc);

	return trayIcon;
}

void UpdateTrayIcon(HWND hwnd, UINT id, HICON icon, const wchar_t* desc) {
	NOTIFYICONDATAW nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hwnd;
	nid.uFlags = NIF_TIP | NIF_ICON;
	nid.uID = id;
	nid.hIcon = icon;
	wcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(wchar_t), desc);
	Shell_NotifyIconW(NIM_MODIFY, &nid);
}


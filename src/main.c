
#include <windows.h>
#include <strsafe.h>
#include <commctrl.h>
#include <ShlObj.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "resource.h"
#include "version.h"
#include "data.h"
#include "tray.h"
#include "listview.h"
#include "utils.h"
#include "setting.h"

static HWND hwndListView;

#define WNDCLASS_NAME L"HWMON_WNDCLASS"
#define TRAYICON_ID 3683
#define TIMER_ID    4074

#define WM_TRAYICON (WM_USER + 101)
UINT WM_TASKBAR_CREATE;

typedef struct {
	const wchar_t* name;
	int id;
	ULONG data;
} ItemInfo;

#define GROUP_INFO 1
#define ITEMS_INFO 5
static ItemInfo infoItems[ITEMS_INFO] = {
	{ L"Model" }, { L"Processor" },  { L"Memory" }, 
	{ L"Revision" }, { L"Serial Number" }
};

#define GROUP_SOFT 2
#define ITEMS_SOFT 3
static ItemInfo softItems[ITEMS_SOFT] = {
	{L"Windows"}, { L"VC Firmware" }, { L"UEFI Firmware" }
};

#define GROUP_TEMP 3
#define ITEMS_TEMP 1
static ItemInfo tempItems[ITEMS_TEMP] = {
	{ L"System" }
};

#define GROUP_CLKS 4
#define ITEMS_CLKS 14
static ItemInfo clockItems[ITEMS_CLKS] = {
	{ L"EMMC" }, { L"UART" }, { L"ARM" }, { L"CORE" }, { L"V3D" },
	{ L"H264" }, { L"ISP" }, { L"SDRAM" }, { L"PIXEL" }, { L"PWM" },
	{ L"HEVC" }, { L"EMMC2" }, { L"M2MC" }, { L"PIXEL_BVB" }
};

#define GROUP_VOLT 5
#define ITEMS_VOLT 4
static ItemInfo voltItems[ITEMS_VOLT] = {
	{ L"CORE" }, { L"SDRAM C" }, { L"SDRAM P" }, { L"SDRAM I" }
};

static const wchar_t* mem_units[] = { L"MB", L"GB" };
static const wchar_t* clk_units[] = { L"Hz", L"MHz", L"GHz" };

void RpiqError(HWND hwnd) {
#ifndef _DEBUG
	MessageBoxW(hwnd,
		L"Got a board revision of zero.\n\n"
		PIMON_APPNAME L" is designed for Windows on Raspberry Pis only.\n"
		L"It will not work on generic Windows PCs.\n\n"
		L"If you are running on a Raspberry Pi, \n"
		L"the RPIQ driver might not be working properly.\n\n"
		L"Try updating your RPIQ driver from\n"
		L"https://github.com/driver1998/bsp/releases.",
		L"RPIQ driver issue", MB_OK | MB_ICONERROR);
#endif
}

void GetInfo(HWND hwnd) {
	wchar_t str[200];
	ULONG rev = GetBoardRevision();

	if (rev == 0) RpiqError(hwnd);

	// Model Name
	LVSetItemText(hwndListView, infoItems[0].id, 1, GetPiModelName(rev));

	// Processor Name
	LVSetItemText(hwndListView, infoItems[1].id, 1, GetProcessorName(rev));

	// Physically installed RAM
	double phyRam = GetInstalledMemory(rev);
	int u1 = PrettyPrintUnits(&phyRam, mem_units, sizeof(mem_units) / sizeof(wchar_t*), 1024.0);

	// RAM available to Windows
	double ram = GetWindowsMemory();
	int u2 = PrettyPrintUnits(&ram, mem_units, sizeof(mem_units) / sizeof(wchar_t*), 1024.0);

	// Memory
	StringCbPrintfW(str, sizeof(str), L"%.3lg %ws (%.3lg %ws Usable)", phyRam, mem_units[u1], ram, mem_units[u2]);
	LVSetItemText(hwndListView, infoItems[2].id, 1, str);

	// Revision
	StringCbPrintfW(str, sizeof(str), L"%X", rev);
	LVSetItemText(hwndListView, infoItems[3].id, 1, str);

	// Serial Number
	ULONGLONG serial = GetSerialNumber();
	StringCbPrintfW(str, sizeof(str), L"%llX", serial);
	LVSetItemText(hwndListView, infoItems[4].id, 1, str);

	// Windows Version
	wchar_t* ver = GetWindowsVersion();
	StringCbPrintfW(str, sizeof(str), ver);
	LVSetItemText(hwndListView, softItems[0].id, 1, str);

	// VC Firmware
	time_t firmware = (time_t)GetFirmwareRevision();
	struct tm time;
	gmtime_s(&time, &firmware);
	StringCbPrintfW(str, sizeof(str), L"%04d-%02d-%02d", 1900 + time.tm_year, time.tm_mon + 1, time.tm_mday);
	LVSetItemText(hwndListView, softItems[1].id, 1, str);

	// UEFI Firmware
	wchar_t* biosVersion = GetBiosVersion();
	LVSetItemText(hwndListView, softItems[2].id, 1, biosVersion);
	free(biosVersion);
}

void UpdateData(HWND hwnd, BOOL force) {
	wchar_t str[20];

	// Temperature
	{
		ULONG value = acpiThermal ? GetTemperatureAcpi() : GetTemperature();
		
		if (value != tempItems[0].data || force) {
			tempItems[0].data = value;
			double temp = acpiThermal ? (value - 2732) / 10.0 : value / 1000.0;
			
			HICON icon = DrawTrayIcon(hwnd, trayBackground, trayForeground, temp);
			StringCbPrintfW(str, sizeof(str), L"Temperature: %.3lg \u2103", temp);
			UpdateTrayIcon(hwnd, TRAYICON_ID, icon, str);
			DestroyIcon(icon);

			StringCbPrintfW(str, sizeof(str), L"%.3lg \u2103", temp);
			LVSetItemText(hwndListView, tempItems[0].id, 1, str);
		}
	}

	// Clocks
	for (ULONG i = 0; i < ITEMS_CLKS; i++) {
		ULONG value = GetClock(i + 1);
		if (value != clockItems[i].data || force) {
			clockItems[i].data = value;
			double clock = value / 1000.0;
			int u = PrettyPrintUnits(&clock, clk_units, sizeof(clk_units) / sizeof(wchar_t*), 1000.0);

			StringCbPrintfW(str, sizeof(str), L"%.3lg %ws", clock, clk_units[u]);
			LVSetItemText(hwndListView, clockItems[i].id, 1, str);
		}
	}

	// Voltages
	for (ULONG i = 0; i < ITEMS_VOLT; i++) {
		ULONG value = GetVoltage(i + 1);
		if (value != voltItems[i].data || force) {
			voltItems[i].data = value;
			double voltage = (value / 1000000.0) * 0.025 + 1.2;

			StringCbPrintfW(str, sizeof(str), L"%.3lg V", voltage);
			LVSetItemText(hwndListView, voltItems[i].id, 1, str);
		}
	}
}

void About(HWND hwnd) {
	MessageBoxW(hwnd,
		PIMON_APPNAME L" Version " PIMON_VERSION L"\n"
		L"Copyright (c) driver1998\n\n"
		L"GitHub: https://github.com/driver1998/PiMon \n"
		L"Release under the MIT License",
		L"About", MB_ICONINFORMATION | MB_OK);
}

void SettingsDialog(HWND hwnd) {
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);
	DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_CONFIG), hwnd, (DLGPROC)SettingsDialogProc);
	UpdateData(hwnd, TRUE);
}

void AlwaysOnTop(HWND hwnd, BOOL value) {
	alwaysOnTop = value;
	ConfigSetInt(PIMON_APPNAME, CONFIG_ALWAYS_ON_TOP, alwaysOnTop);

	HMENU menu = GetSystemMenu(hwnd, FALSE);
	CheckMenuItem(menu, IDM_SYS_ALWAYS_TOP, alwaysOnTop ? MF_CHECKED : MF_UNCHECKED);

	SetWindowPos(hwnd, alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void HideOnMinimize(HWND hwnd, BOOL value) {
	hideOnMinimize = value;
	ConfigSetInt(PIMON_APPNAME, CONFIG_HIDE_ON_MINIMIZE, hideOnMinimize);

	HMENU menu = GetSystemMenu(hwnd, FALSE);
	CheckMenuItem(menu, IDM_SYS_HIDE_MINIMIZE, hideOnMinimize ? MF_CHECKED : MF_UNCHECKED);

	LONG style = GetWindowLongW(hwnd, GWL_STYLE);
	if (hideOnMinimize && (style & WS_MINIMIZE)) {
		ShowWindow(hwnd, SW_HIDE);
	}
	else if (!hideOnMinimize && !(style & WS_VISIBLE)) {
		ShowWindow(hwnd, SW_SHOWNORMAL);
	}
}

void InitListView(HWND hwnd) {
	LVAddColumn(hwndListView, 0, L"Description", 120);
	LVAddColumn(hwndListView, 1, L"Value", 240);
	ListView_EnableGroupView(hwndListView, TRUE);
	ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	UINT dpi = GetDpiForWindow(hwnd);
	HIMAGELIST imageList = ImageList_Create(SCALE(16, dpi), SCALE(16, dpi), ILC_COLOR32, 2, 0);
	ListView_SetImageList(hwndListView, imageList, LVSIL_SMALL);

	// Info
	LVAddGroup(hwndListView, -1, L"Device Info", GROUP_INFO);
	for (ULONG i = 0; i < ITEMS_INFO; i++) {
		infoItems[i].id = LVAddItem(hwndListView, -1, infoItems[i].name);
		LVSetItemGroupId(hwndListView, infoItems[i].id, GROUP_INFO);
	}

	// Software Version
	LVAddGroup(hwndListView, -1, L"Software", GROUP_SOFT);
	for (ULONG i = 0; i < ITEMS_SOFT; i++) {
		softItems[i].id = LVAddItem(hwndListView, -1, softItems[i].name);
		LVSetItemGroupId(hwndListView, softItems[i].id, GROUP_SOFT);
	}

	// Temperature
	LVAddGroup(hwndListView, -1, L"Temperature", GROUP_TEMP);
	for (ULONG i = 0; i < ITEMS_TEMP; i++) {
		tempItems[i].id = LVAddItem(hwndListView, -1, tempItems[i].name);
		LVSetItemGroupId(hwndListView, tempItems[i].id, GROUP_TEMP);
		LVSetItemText(hwndListView, tempItems[0].id, 1, L"0 \u2103");
	}
	
	// Clocks
	LVAddGroup(hwndListView, -1, L"Clocks", GROUP_CLKS);
	for (ULONG i = 0; i < ITEMS_CLKS; i++) {
		clockItems[i].id = LVAddItem(hwndListView, -1, clockItems[i].name);
		LVSetItemGroupId(hwndListView, clockItems[i].id, GROUP_CLKS);
		LVSetItemText(hwndListView, clockItems[i].id, 1, L"0 Hz");
	}

	// Voltages
	LVAddGroup(hwndListView, -1, L"Voltages", GROUP_VOLT);
	for (ULONG i = 0; i < ITEMS_VOLT; i++) {
		voltItems[i].id =  LVAddItem(hwndListView, -1, voltItems[i].name);
		LVSetItemGroupId(hwndListView, voltItems[i].id, GROUP_VOLT);
		LVSetItemText(hwndListView, voltItems[i].id, 1, L"0 V");
	}
}

void InitTrayIcon(HWND hwnd) {
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);
	HICON icon = LoadIconW(hInstance, MAKEINTRESOURCEW(10));
	AddTrayIcon(hwnd, TRAYICON_ID, PIMON_APPNAME, icon, WM_TRAYICON);
}

void InitSysMenu(HWND hwnd) {
	HMENU menu = GetSystemMenu(hwnd, FALSE);
	AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(menu, MF_STRING, IDM_SYS_ALWAYS_TOP, L"Always on &Top");
	AppendMenuW(menu, MF_STRING, IDM_SYS_HIDE_MINIMIZE, L"&Hide on Minimize");
	AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(menu, MF_STRING, IDM_SYS_SETTINGS, L"S&ettings...");
	AppendMenuW(menu, MF_STRING, IDM_SYS_ABOUT, L"&About...");
	CheckMenuItem(menu, IDM_SYS_ALWAYS_TOP, alwaysOnTop ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(menu, IDM_SYS_HIDE_MINIMIZE, hideOnMinimize ? MF_CHECKED : MF_UNCHECKED);
}

void OnCreate(HWND hwnd, CREATESTRUCTW* c) {
	ResizeWindowByClientArea(hwnd, 460, 590);
	MoveWindowToCenterOfScreen(hwnd);

	hwndListView = CreateWindowExW(
		0,
		WC_LISTVIEWW,
		L"ListView",
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | 
		WS_TABSTOP | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_REPORT,
		0, 0, 250, 500,
		hwnd,
		NULL,
		c->hInstance,
		NULL
	);

	InitTrayIcon(hwnd);
	InitSysMenu(hwnd);
	InitListView(hwnd);

	GetInfo(hwnd);
	UpdateData(hwnd, TRUE);

	SetTimer(hwnd, TIMER_ID, 1000, NULL);
}

void OnResize(HWND hwnd, WPARAM wParam, WORD client_width, WORD client_height) {
	if (hideOnMinimize && wParam == SIZE_MINIMIZED) {
		ShowWindow(hwnd, SW_HIDE);
		return;
	}

	UINT dpi = GetDpiForWindow(hwnd);
	MoveWindow(hwndListView, 0, 0, client_width, client_height, TRUE);

	LVSetColumnWidth(hwndListView, 0, SCALE(120, dpi));

	int valueWidth = client_width - SCALE(120, dpi) - GetSystemMetricsForDpi(SM_CXVSCROLL, dpi);
	LVSetColumnWidth(hwndListView, 1, valueWidth);
}

void OnDpiChanged(HWND hwnd, WORD dpi, RECT* r) {
	MoveWindow(hwnd, r->left, r->top, (r->right - r->left), (r->bottom - r->top), TRUE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// Explorer restarted, add tray icon again
	if (uMsg == WM_TASKBAR_CREATE) {
		InitTrayIcon(hwnd);
		UpdateData(hwnd, TRUE);
	}
	
	switch (uMsg) {
	case WM_CREATE:
		OnCreate(hwnd, (CREATESTRUCTW*)lParam);
		break;
	case WM_SIZE:
		OnResize(hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DPICHANGED:
		OnDpiChanged(hwnd, HIWORD(wParam), (RECT*)lParam);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == 0) {
			switch (LOWORD(wParam)) {
			case IDM_ABOUT:
				About(hwnd);
				break;
			case IDM_SETTINGS:
				SettingsDialog(hwnd);
				break;
			case IDM_ALWAYS_TOP:
				AlwaysOnTop(hwnd, !alwaysOnTop);
				break;
			case IDM_HIDE_MINIMIZE:
				HideOnMinimize(hwnd, !hideOnMinimize);
				break;
			case IDM_EXIT:
				SendMessageW(hwnd, WM_CLOSE, 0, 0);
				break;
			}
		}
		break;
	case WM_TRAYICON:
		switch (lParam) {
		case WM_LBUTTONDBLCLK: {
			SetForegroundWindow(hwnd);
			LONG style = GetWindowLongW(hwnd, GWL_STYLE);
			if (style & WS_MINIMIZE || !(style & WS_VISIBLE))
				ShowWindow(hwnd, SW_SHOWNORMAL);
			break;
		}	
		case WM_RBUTTONUP: {
			HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);
			POINT p; GetCursorPos(&p);

			HMENU menu = LoadMenuW(hInstance, MAKEINTRESOURCE(IDM_TRAY));
			HMENU subMenu = GetSubMenu(menu, 0);

			CheckMenuItem(subMenu, IDM_ALWAYS_TOP, alwaysOnTop ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem(subMenu, IDM_HIDE_MINIMIZE, hideOnMinimize ? MF_CHECKED : MF_UNCHECKED);

			SetForegroundWindow(hwnd);
			TrackPopupMenu(subMenu, TPM_BOTTOMALIGN, p.x, p.y, 0, hwnd, NULL);
			DestroyMenu(menu);
			break;
		}
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam) {
		case IDM_SYS_ABOUT:
			About(hwnd);
			break;
		case IDM_SYS_SETTINGS:
			SettingsDialog(hwnd);
			break;
		case IDM_SYS_ALWAYS_TOP:
			AlwaysOnTop(hwnd, !alwaysOnTop);
			break;
		case IDM_SYS_HIDE_MINIMIZE:
			HideOnMinimize(hwnd, !hideOnMinimize);
			break;
		}
		break;
	case WM_TIMER:
		UpdateData(hwnd, FALSE);
		break;
	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

_Use_decl_annotations_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

	LoadConfig();

#ifdef _DEBUG
	FILE* stream;
	AllocConsole();
	_wfreopen_s(&stream, L"CONOUT$", L"w", stdout);
	wprintf(PIMON_APPNAME L" v" PIMON_VERSION L"\n");
#endif

	// Register the Explorer restart message
	WM_TASKBAR_CREATE = RegisterWindowMessageW(L"TaskbarCreated");

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WNDCLASS_NAME;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_MAIN));
	RegisterClassW(&wc);

	HWND hwnd = CreateWindowExW(
		0,
		WNDCLASS_NAME,
		PIMON_APPNAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, 
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL) return -1;

	ShowWindow(hwnd, nCmdShow);

	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, 0, 0)) {
		if (!IsDialogMessageW(hwnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	KillTimer(hwnd, TIMER_ID);
	DeleteTrayIcon(hwnd, TRAYICON_ID);
	return 0;
}
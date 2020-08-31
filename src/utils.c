#include <windows.h>
#include "utils.h"

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

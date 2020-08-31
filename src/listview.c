#include <windows.h>
#include <CommCtrl.h>
#include "listview.h"

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

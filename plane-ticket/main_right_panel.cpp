#include "main.h"

#include <CommCtrl.h>

// Right Panel
HWND hWndRightPanel;

static inline void right_panel_add_buttons(int parent_x, int parent_y, int parent_w, int parent_h)
{
	const int WIDTH = parent_w - 2 * RIGHT_PANEL_PADDING;

	int x = RIGHT_PANEL_PADDING, y = RIGHT_PANEL_PADDING + 24;

	HWND hWndButtonAdd = CreateWindow(WC_BUTTON, L"New Flight",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		x,
		y,
		WIDTH,
		RIGHT_PANEL_BTN_HEIGHT,
		hWndRightPanel, (HMENU)IDC_BUTTON_ADD_FLIGHT, hInst, 0);
}

BOOL CALLBACK refine_font_proc(HWND hWnd, LPARAM lParam)
{
	HFONT hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(TRUE, 0));
	return TRUE;
}

LRESULT CALLBACK RightPanelMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR subClass, DWORD_PTR data)
{
	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_ADD_FLIGHT:
			show_add_flight_dialog();
			break;
		}
		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hwnd, RightPanelMsgProc, 0);
		break;
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void create_right_panel(HWND hwndParent, int x, int y, int w, int h)
{
	hWndRightPanel = CreateWindow(WC_BUTTON, L"Flight Action",
		WS_CHILD | WS_VISIBLE | WS_GROUP | BS_GROUPBOX,
		x,
		y,
		w,
		h,
		hwndParent, NULL, hInst, 0);

	if (hWndRightPanel == NULL)
	{
		MessageBox(hMainWnd, L"Cannot initialize UI.", L"Failure", MB_OK | MB_ICONERROR);
		exit(1);
	}

	right_panel_add_buttons(x, y, w, h);

	EnumChildWindows(hWndRightPanel, refine_font_proc, 0);
	refine_font_proc(hWndRightPanel, 0); // call manually for itself

	SetWindowSubclass(hWndRightPanel, RightPanelMsgProc, 0, 0);
}


#include "main.h"

#include <CommCtrl.h>
#include <windowsx.h>

HWND hStatusBar;


void create_status_bar()
{
	hStatusBar = CreateWindowEx(
		0,                       // no extended styles
		STATUSCLASSNAME,         // name of status bar class
		L"Ready",           // no text when first created
		SBARS_SIZEGRIP |         // includes a sizing grip
		WS_CHILD | WS_VISIBLE,   // creates a visible child window
		0, 0, 0, 0,              // ignores size and position
		hMainWnd,              // handle to parent window
		(HMENU)IDC_MAINSTATUSBAR,       // child window identifier
		hInst,                   // handle to application instance
		NULL);                   // no window creation data
}

void status_bar_resize()
{
	SendMessage(hStatusBar, WM_SIZE, 0, 0);
}

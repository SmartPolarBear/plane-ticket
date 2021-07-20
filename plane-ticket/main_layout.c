#include "main.h"

void make_main_layout()
{
	RECT rcClient;
	GetClientRect(hMainWnd, &rcClient);

	create_right_panel(hMainWnd,
		rcClient.right - RIGHT_PANEL_WIDTH - RIGHT_PANEL_MARGIN,
		RIGHT_PANEL_MARGIN,
		RIGHT_PANEL_WIDTH,
		rcClient.bottom - rcClient.top);

	create_main_listview(hMainWnd,
		0, 0,
		rcClient.right - rcClient.left - 2 * RIGHT_PANEL_MARGIN - RIGHT_PANEL_WIDTH,
		rcClient.bottom - rcClient.top);

}
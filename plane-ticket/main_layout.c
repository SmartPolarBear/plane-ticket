#include "main.h"

void make_main_layout()
{
	RECT rcClient;
	GetClientRect(hMainWnd, &rcClient);

	create_right_panel(hMainWnd, 0, 0, 0, 0);

	create_main_listview(hMainWnd,
		0, 0,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top);

}
#include "main.h"

void make_main_layout()
{
	RECT rcClient;
	GetClientRect(hMainWnd, &rcClient);

	create_status_bar();

	RECT rc_status_bar;
	GetClientRect(hStatusBar, &rc_status_bar);

	create_right_panel(hMainWnd);

	create_main_listview(hMainWnd,
		0, 0,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top - (rc_status_bar.bottom - rc_status_bar.top));

}

void update_main_layout()
{
	status_bar_resize();
	main_listview_resize();
}
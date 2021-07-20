#include "main.h"

#include "add_plane_dlg.h"
#include "about_dlg.h"

void show_add_flight_dialog()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ADDPLANEDLG), hMainWnd, AddPlaneDialog);
	load_main_listview();
}

void show_about_dialog()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hMainWnd, AboutDialog);
}
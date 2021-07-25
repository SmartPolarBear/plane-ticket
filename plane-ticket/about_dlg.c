#include "framework.h"
#include "about_dlg.h"

#include "main.h"
#include "resource.h"

// Message handler for about box.
INT_PTR CALLBACK AboutDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


DWORD show_about_dialog(HWND h_parent)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hMainWnd, AboutDialog);
}
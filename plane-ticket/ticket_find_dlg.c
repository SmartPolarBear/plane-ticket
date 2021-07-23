#include "ticket_find_dlg.h"

#include "main.h"

INT_PTR CALLBACK TicketFindWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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


WPARAM ticket_find_dialog(HWND hDlg)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FIND_SEAT), hDlg, TicketFindWndProc);
}


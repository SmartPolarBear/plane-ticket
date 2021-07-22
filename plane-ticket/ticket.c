#include "framework.h"

#include "ticket.h"
#include "main.h"



INT_PTR CALLBACK TicketWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		create_ticket_listview(hDlg);

		wchar_t *title_buf=calloc(64,sizeof(wchar_t));
		memset(title_buf, 0, sizeof(title_buf));
		wsprintf(title_buf, L"Tickets of %s ", doc.result[main_list_view_selected].id);
		SetWindowText(hDlg, title_buf);
		free(title_buf);

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

void show_ticket_details()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_TICKET_DIALOG), hMainWnd, TicketWndProc);
	load_document(&doc);
}

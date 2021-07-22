#include "framework.h"

#include "ticket_booking.h"

#include "ticket.h"
#include "main.h"

#include "resource.h"

#include <windowsx.h>

flight_info_t* target_flight = NULL;

INT_PTR CALLBACK TicketWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		create_ticket_listview(hDlg);

		wchar_t* title_buf = calloc(64, sizeof(wchar_t));
		if (!title_buf)
		{
			MessageBox(hMainWnd, L"Insufficient memory!", L"Failure", MB_OK | MB_ICONERROR);
			exit(1);
		}

		memset(title_buf, 0, sizeof(title_buf));
		wsprintf(title_buf, L"Tickets of %s ", doc.result[main_list_view_selected]->id);
		SetWindowText(hDlg, title_buf);
		free(title_buf);

		target_flight = malloc(sizeof(flight_t));
		if (!target_flight)
		{
			MessageBox(hMainWnd, L"Insufficient memory!", L"Failure", MB_OK | MB_ICONERROR);
			exit(1);
		}

		int ret = document_get_flight_info(doc.result[main_list_view_selected], target_flight);
		if (ret)
		{
			MessageBox(hMainWnd, L"Cannot load flight data!", L"Failure", MB_OK | MB_ICONERROR);
			EndDialog(hDlg, LOWORD(wParam));
		}

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_BOOK:
			show_booking_dialog();
			break;
		default:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		break;

	case WM_DESTROY:
		free(target_flight);
		break;
	}
	return (INT_PTR)FALSE;
}

void show_ticket_details()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_TICKET_DIALOG), hMainWnd, TicketWndProc);
	load_document(&doc);
}

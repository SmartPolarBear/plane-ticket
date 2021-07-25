#include "framework.h"

#include "ticket_booking_dlg.h"
#include "ticket_find_dlg.h"
#include "ticket_details_dlg.h"

#include "ticket.h"
#include "main.h"

#include "resource.h"

#include <windowsx.h>

flight_info_t* target_flight = NULL;


int ticket_listview_selected = -1;

static inline void update_controls(HWND hDlg)
{
	Button_Enable(GetDlgItem(hDlg, IDC_BUTTON_REFOUND), (ticket_listview_selected >= 0));
	Button_Enable(GetDlgItem(hDlg, IDC_BUTTON_DETAILS), (ticket_listview_selected >= 0));
}

static inline void ticket_book(HWND hDlg)
{
	if (show_booking_dialog(hDlg, target_flight) == IDOK)
	{
		load_ticket_listview(hDlg);
	}
}

static inline void ticket_find(HWND hDlg)
{
	if (ticket_find_dialog(hDlg) != IDCANCEL)
	{
		load_ticket_listview(hDlg);
	}
}

static inline void ticket_details(HWND hDlg)
{
	show_ticket_details_dlg(hDlg, target_flight->result[ticket_listview_selected]);
}

static inline void ticket_refound(HWND hDlg)
{
	if (ticket_listview_selected < 0)return;

	wchar_t* notify_text = calloc(256, sizeof(wchar_t));
	if (!notify_text)
	{
		MessageBox(hMainWnd, L"Insufficient memory!", L"Failure", MB_OK | MB_ICONERROR);
		exit(1);
	}

	ticket_t* selected = target_flight->result[ticket_listview_selected];


	swprintf(notify_text, 256, L"Are you sure to refound the ticket?\nID: %lld\nName:%s\nID:%s\n",
		selected->id,
		selected->owner,
		selected->owner_id);

	int msg_id = MessageBox(hMainWnd, notify_text, L"Confirm", MB_ICONQUESTION | MB_YESNO);
	if (msg_id == IDNO)
	{
		free(notify_text);
		return;
	}
	free(notify_text);


	int ret = document_flight_refound_ticket(target_flight, selected);
	if (ret)
	{
		MessageBox(hDlg, L"Cannot refound ticket", L"Failure", MB_OK | MB_ICONERROR);
		load_ticket_listview(hDlg);
		return;
	}

	ret = documeent_flight_info_save(&doc, target_flight);
	if (ret)
	{
		MessageBox(hDlg, L"Cannot save flight information", L"Failure", MB_OK | MB_ICONERROR);
		load_ticket_listview(hDlg);
	}

	ListView_DeleteItem(hWndMainListView, main_list_view_selected);

	main_list_view_selected = -1;

	load_ticket_listview(hDlg);
}

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

		target_flight = malloc(sizeof(flight_info_t));
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

		load_ticket_listview(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_BOOK:
			ticket_book(hDlg);
			break;

		case IDC_BUTTON_REFOUND:
			ticket_refound(hDlg);
			break;

		case IDC_BUTTON_FIND:
			ticket_find(hDlg);
			break;

		case IDC_BUTTON_DETAILS:
			ticket_details(hDlg);
			break;

		default:
			if (LOWORD(wParam) == IDOK)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			else if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		break;

	case WM_NOTIFY:

		switch (LOWORD(wParam))
		{
		case IDC_LISTTICKET:
			HWND h_ticket_listview = GetDlgItem(hDlg, IDC_LISTTICKET);
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_CLICK:
				ticket_listview_selected = SendMessage(h_ticket_listview, LVM_GETNEXTITEM,
					-1, LVNI_FOCUSED | LVNI_SELECTED);
				break;
			case NM_DBLCLK:
				ticket_listview_selected = SendMessage(h_ticket_listview, LVM_GETNEXTITEM,
					-1, LVNI_FOCUSED | LVNI_SELECTED);

				ticket_details(hDlg);
				break;
			case NM_RCLICK:
				ticket_listview_selected = SendMessage(h_ticket_listview, LVM_GETNEXTITEM,
					-1, LVNI_FOCUSED | LVNI_SELECTED);
				break;
			}
			update_controls(hDlg);
			break;
		}
		break;


	case WM_DESTROY:
		free(target_flight);
		ticket_listview_selected = -1;
		target_flight = NULL;
		break;
	}
	return (INT_PTR)FALSE;
}

void show_ticket_dialog()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_TICKET_DIALOG), hMainWnd, TicketWndProc);
	load_main_listview();
}

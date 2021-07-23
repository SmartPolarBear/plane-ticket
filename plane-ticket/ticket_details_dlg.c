#include "ticket_details_dlg.h"

#include "main.h"

#include <windowsx.h>

static ticket_t* the_ticket = NULL;

static inline void main_list_put_line(HWND hDlg, wchar_t* before_colon, wchar_t* after_colon)
{
	wchar_t buf[256] = { 0 };

	HWND h_main_list = GetDlgItem(hDlg, IDC_LIST_MAIN);

	memset(buf, 0, sizeof(buf));
	wsprintf(buf, L"%s: %s", before_colon, after_colon);
	ListBox_AddString(h_main_list, buf);
}

static inline void main_list_put_line_int(HWND hDlg, wchar_t* before_colon, uint64_t after_colon)
{
	wchar_t buf[256] = { 0 };

	HWND h_main_list = GetDlgItem(hDlg, IDC_LIST_MAIN);

	memset(buf, 0, sizeof(buf));
	swprintf(buf, sizeof(buf) / sizeof(buf[0]), L"%ls: %lld", before_colon, after_colon);
	ListBox_AddString(h_main_list, buf);
}

INT_PTR CALLBACK TicketDetailsWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	wchar_t buf[256] = { 0 };

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		memset(buf, 0, sizeof(buf));
		swprintf(buf, sizeof(buf) / sizeof(buf[0]), L"Ticket No. %lld", the_ticket->id);
		SetWindowText(hDlg, buf);


		main_list_put_line(hDlg, L"Name", the_ticket->owner);
		main_list_put_line(hDlg, L"ID", the_ticket->owner_id);
		main_list_put_line_int(hDlg, L"Phone", the_ticket->owner_phone);
		main_list_put_line(hDlg, L"Seat", the_ticket->seat);

		if (the_ticket->flags & TFLAG_CLASS_FIRST)
		{
			main_list_put_line(hDlg, L"Class", L"First");

		}
		else if (the_ticket->flags & TFLAG_CLASS_BUSINESS)
		{
			main_list_put_line(hDlg, L"Class", L"Business");

		}
		else if (the_ticket->flags & TFLAG_CLASS_ECONOMY)
		{
			main_list_put_line(hDlg, L"Class", L"Economy");
		}

		if (the_ticket->flags & TFLAG_UNACCOMPANIED_CHILDREN)
		{
			main_list_put_line(hDlg, L"Special", L"Unaccompanied Child");
		}

		if (the_ticket->flags & TFLAG_MEDICAL_NEEDS)
		{
			main_list_put_line(hDlg, L"Special", L"Medical Needs");
		}

		if (the_ticket->flags & TFLAG_FOOD_MUSLIM)
		{
			main_list_put_line(hDlg, L"Food", L"Muslim");

		}
		else if (the_ticket->flags & TFLAG_FOOD_VEGETARIAN)
		{
			main_list_put_line(hDlg, L"Food", L"Vegetarian");
		}
		else 
		{
			main_list_put_line(hDlg, L"Food", L"Normal");
		}

		SetWindowText(GetDlgItem(hDlg, IDC_EDIT_NOTES), the_ticket->notes);

		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	case WM_DESTROY:
		the_ticket = NULL;
		break;
	}
	return (INT_PTR)FALSE;
}


WPARAM show_ticket_details_dlg(HWND hDlg, ticket_t* t)
{
	the_ticket = t;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_TICKET_DETAILS), hDlg, TicketDetailsWndProc);
}

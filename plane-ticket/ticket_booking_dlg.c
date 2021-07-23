#include "ticket_booking_dlg.h"

#include "main.h"
#include "ticket.h"

#include "resource.h"

#include "helper.h"

#include <windowsx.h>

#include <math.h>

#define CONTROL_TEXT_TO_BUF(id,buf,buf_len) \
do \
{ \
DWORD len = (WORD)SendDlgItemMessage(hDlg, \
	id, \
	EM_LINELENGTH, \
	(WPARAM)0, \
	(LPARAM)0); \
GetDlgItemText(hDlg, id, buf, buf_len); \
buf[len] = 0; \
}while(0) \




static inline void load_class_combo(HWND hDlg)
{
	HWND h_class_combo = GetDlgItem(hDlg, IDC_CLASS_COMBO);

	if (target_flight->parent->flags & FFLAG_CLASS_HAS_FIRST)
	{
		SendMessage(h_class_combo, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"First");
	}

	if (target_flight->parent->flags & FFLAG_CLASS_HAS_BUSINESS)
	{
		SendMessage(h_class_combo, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Business");
	}

	if (target_flight->parent->flags & FFLAG_CLASS_HAS_ECONOMY)
	{
		SendMessage(h_class_combo, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Economy");
	}

	SendMessage(h_class_combo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}


static inline int get_rows()
{
	return (int)ceil((target_flight->parent->remaining + target_flight->parent->sold) / 6.0);
}

static inline void add_available_seat_for_row(HWND h_seat_combo, uint8_t row, int idx)
{
	wchar_t buf[16] = { 0 };
	const wchar_t rows[] = { 'A','B','C','H','J','K' };

	for (int i = 0; i < 6; i++)
	{
		if ((row & (1 << i)) == 0)
		{
			wsprintf(buf, L"%d%c", idx, rows[i]);
			SendMessage(h_seat_combo, CB_ADDSTRING, (WPARAM)0, (LPARAM)buf);
			memset(buf, 0, sizeof(buf));
		}
	}
}

static inline void load_seat_combo(HWND hDlg)
{
	HWND h_seat_combo = GetDlgItem(hDlg, IDC_COMBO_SEAT),
		h_class_combo = GetDlgItem(hDlg, IDC_CLASS_COMBO);

	// determine the class use first char
	wchar_t buf[2] = { 0 };
	GetWindowText(h_class_combo, buf, 2);

	int rows = get_rows(), row_start = 1;
	switch (buf[0])
	{
	case 'F': // First
		rows = 3;
		row_start = 1;
		break;
	case 'B':// Business
		rows = 3;
		row_start = 4;
		break;
	case 'E': // Economy
		rows = get_rows() - 6;
		row_start = 7;
		break;
	}

	ComboBox_ResetContent(h_seat_combo);

	for (int i = row_start; i - row_start < rows; i++)
	{
		add_available_seat_for_row(h_seat_combo, target_flight->parent->ticket_bitmap[i], i);
	}
	SendMessage(h_seat_combo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

static inline void load_seat_info(HWND hDlg)
{
	load_class_combo(hDlg);

	load_seat_combo(hDlg);
}


static inline void update_button_status()
{

}

static inline int do_book_flight(HWND hDlg)
{
	ticket_t t;
	memset(&t, 0, sizeof(t));

	GetWindowText(GetDlgItem(hDlg, IDC_COMBO_SEAT), t.seat, 8);

	// determine the class use first char
	wchar_t class[2] = { 0 };
	GetWindowText(GetDlgItem(hDlg, IDC_CLASS_COMBO), class, 2);
	switch (class[0])
	{
	case 'F': // First
		t.flags |= TFLAG_CLASS_FIRST;
		break;
	case 'B':// Business
		t.flags |= TFLAG_CLASS_BUSINESS;
		break;
	case 'E': // Economy
		t.flags |= TFLAG_CLASS_BUSINESS;
		break;
	}

	CONTROL_TEXT_TO_BUF(IDC_EDIT_NAME, t.owner, 32);
	CONTROL_TEXT_TO_BUF(IDC_EDIT_ID, t.owner_id, 32);

	wchar_t phone_buf[15] = { 0 }, * end = NULL;
	CONTROL_TEXT_TO_BUF(IDC_EDIT_PHONE, phone_buf, 11);
	t.owner_phone = wcstol(phone_buf, &end, 10);

	CONTROL_TEXT_TO_BUF(phone_buf, t.notes, 256);


	if (IsDlgButtonChecked(hDlg, IDC_UNACCOMPANIED_CHILDREN))
	{
		t.flags |= TFLAG_UNACCOMPANIED_CHILDREN;
	}

	if (IsDlgButtonChecked(hDlg, IDC_CHECK_MEDICAL))
	{
		t.flags |= TFLAG_MEDICAL_NEEDS;
	}

	if (IsDlgButtonChecked(hDlg, IDC_RADIO_FMUSLIM))
	{
		t.flags |= TFLAG_FOOD_MUSLIM;
	}

	if (IsDlgButtonChecked(hDlg, IDC_RADIO_FMUSLIM))
	{
		t.flags |= IDC_RADIO_FVEGE;
	}

	t.id = (uint32_t)hashing(t.owner_id);

	int ret = document_flight_book_ticket(target_flight, &t);
	if (ret)
	{
		MessageBox(hDlg, L"Cannot book ticket", L"Failure", MB_OK | MB_ICONERROR);
		return ret;
	}

	ret = documeent_flight_info_save(&doc, target_flight);
	if (ret)
	{
		MessageBox(hDlg, L"Cannot save flight information", L"Failure", MB_OK | MB_ICONERROR);
		return ret;
	}


	return ret;
}


INT_PTR CALLBACK TicketBookingWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT_FLIGHT_ID), target_flight->parent->id);
		//TODO: set date

		load_seat_info(hDlg);

		Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_FNORMAL), TRUE);

		Edit_LimitText(GetDlgItem(hDlg, IDC_EDIT_NOTES), 256);

		return (INT_PTR)TRUE;

	case WM_COMMAND:

		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
			if (LOWORD(wParam) == IDC_EDIT_NOTES)
			{
				HWND h_notes_edit = ((HWND)lParam);
				wchar_t buf[32] = { 0 };

				wsprintf(buf, L"%d / 256", GetWindowTextLength(h_notes_edit));
				SetWindowText(GetDlgItem(hDlg, IDC_STATIC_NOTES_LENGTH), buf);
			}
			break;
		case CBN_SELCHANGE:
			if (LOWORD(wParam) == IDC_CLASS_COMBO)
			{
				load_seat_combo(hDlg);
			}
			break;
		default:
			if (LOWORD(wParam) == IDOK)
			{
				int ret = do_book_flight(hDlg);
				if (ret == 0)
				{
					EndDialog(hDlg, LOWORD(wParam));
				}
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
	}
	return (INT_PTR)FALSE;
}


WPARAM show_booking_dialog(HWND hDlg, flight_info_t* target)
{
	target_flight = target;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_BOOKING), hDlg, TicketBookingWndProc);
}
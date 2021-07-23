#define _CRT_SECURE_NO_WARNINGS

#include "ticket_find_dlg.h"

#include "main.h"
#include "ticket.h"

#include "ticket_booking_dlg.h"

#include <windowsx.h>
#include <intrin.h>

static int result_list_selected = -1;

static inline void load_class_combo(HWND hDlg)
{
	HWND h_class_combo = GetDlgItem(hDlg, IDC_CLASS_COMBO);

	int i = 0;
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

wchar_t* conds[] = { L"Beside Window" ,L"Beside Corridor",L"2 Adjacent",L"3 Adjacent" };

static inline void load_condition_combo(HWND hDlg)
{
	HWND h_cond_combo = GetDlgItem(hDlg, IDC_COND_COMBO);
	for (int i = 0; i < sizeof(conds) / sizeof(conds[0]); i++)
	{
		SendMessage(h_cond_combo, CB_ADDSTRING, (WPARAM)0, (LPARAM)conds[i]);
	}
	SendMessage(h_cond_combo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

}

static inline void do_find_by_mask_range(HWND hDlg, int pos_mask, int start, int end)
{
	HWND h_result_list = GetDlgItem(hDlg, IDC_RESULT_LIST);

	wchar_t buf[6][64] = { 0 }, * cols[] = { L"A",L"B",L"C",L"H",L"J",L"K" };
	for (int i = start; i <= end; i++)
	{
		if (((~target_flight->parent->ticket_bitmap[i]) & pos_mask) == pos_mask)
		{
			for (int j = 0, idx = 0; j < 6; j++)
			{
				if (pos_mask & (1 << j))
				{
					swprintf(buf[idx], sizeof(buf[idx]) / sizeof(buf[idx][0]), L"%d%ls", i, cols[j]);
					idx++;
				}
			}

			wchar_t full_buf[256] = { 0 };
			swprintf(full_buf, sizeof(full_buf) / sizeof(full_buf[0]), L"%ls %ls %ls %ls %ls %ls", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

			ListBox_AddString(h_result_list, full_buf);
		}
	}
}

static inline void do_find_by_mask(HWND hDlg, int pos_mask)
{
	HWND h_class_combo = GetDlgItem(hDlg, IDC_CLASS_COMBO);
	// determine the class use first char
	wchar_t buf[2] = { 0 };
	GetWindowText(h_class_combo, buf, 2);

	int rows = document_flight_get_rows(target_flight), row_start = 1;
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
		rows = document_flight_get_rows(target_flight) - 6;
		row_start = 7;
		break;
	}

	do_find_by_mask_range(hDlg, pos_mask, row_start, row_start + rows - 1);
}

static inline void do_find(HWND hDlg)
{
	HWND h_result_list = GetDlgItem(hDlg, IDC_RESULT_LIST);

	ListBox_ResetContent(h_result_list);


	int sel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COND_COMBO));
	switch (sel)
	{
	case 0:
		do_find_by_mask(hDlg, 0b000001);
		do_find_by_mask(hDlg, 0b100000);
		break;
	case 1:
		do_find_by_mask(hDlg, 0b000100);
		do_find_by_mask(hDlg, 0b001000);
		break;
	case 2:
		for (int mask = 0b11; mask != 0b110000; mask <<= 1)
		{
			do_find_by_mask(hDlg, mask);
		}
		break;
	case 3:
		for (int mask = 0b111; mask != 0b111000; mask <<= 1)
		{
			do_find_by_mask(hDlg, mask);
		}
		break;
	}
}

static inline void update_control_status(HWND hDlg)
{
	Button_Enable(GetDlgItem(hDlg, IDC_BUTTON_BOOK), result_list_selected >= 0);
}

static inline int get_class_flags(HWND hDlg)
{
	HWND h_class_combo = GetDlgItem(hDlg, IDC_CLASS_COMBO);

	// determine the class use first char
	wchar_t buf[2] = { 0 };
	GetWindowText(h_class_combo, buf, 2);

	switch (buf[0])
	{
	case 'F': // First
		return TFLAG_CLASS_FIRST;
	case 'B':// Business
		return TFLAG_CLASS_BUSINESS;
	case 'E': // Economy
		return TFLAG_CLASS_ECONOMY;
	}
	return 0;
}

INT_PTR CALLBACK TicketFindWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		load_class_combo(hDlg);
		load_condition_combo(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_FIND:

			do_find(hDlg);

			break;

		case IDC_BUTTON_BOOK:

			wchar_t buf[64] = { 0 }, buf2[64] = { 0 };
			ListBox_GetText(GetDlgItem(hDlg, IDC_RESULT_LIST), result_list_selected, buf);
			int row = 0, target_flag = get_class_flags(hDlg);
			wchar_t col = 0;

			wchar_t* pbuf = buf;
			while (swscanf(pbuf, L"%d%lc", &row, &col) == 2)
			{
				swprintf(buf2, sizeof(buf2) / sizeof(buf2[0]), L"%d%lc", row, col);
				show_booking_dialog_target_seat(hDlg, target_flight, buf2, target_flag);

				while (*pbuf && *pbuf != ' ')
					pbuf++;

				pbuf++;
			}
			break;

		case IDC_RESULT_LIST:

			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				result_list_selected = ListBox_GetCurSel((HWND)lParam);
				update_control_status(hDlg);
			default:
				break;
			}

			break;

		default:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		}
		break;

	case WM_NOTIFY:
		break;
	}
	return (INT_PTR)FALSE;
}


WPARAM ticket_find_dialog(HWND hDlg)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_FIND_SEAT), hDlg, TicketFindWndProc);
}


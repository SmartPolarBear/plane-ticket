#include "main.h"

#include <CommCtrl.h>

#include "resource.h"

#include "main.h"

#include <windowsx.h>

// Right Panel
HWND h_query_tool_window;


static inline void do_query_date(HWND hDlg)
{
	flight_date_t* from = malloc(sizeof(flight_date_t));
	if (!from)
	{
		return;
	}

	flight_date_t* to = malloc(sizeof(flight_date_t));
	if (!to)
	{
		goto free_from;
	}

	HWND h_date_from_picker = GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE_FROM),
		h_date_to_picker = GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE_TO),
		h_time_from_picker = GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME_FROM),
		h_time_to_picker = GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME_TO);

	SYSTEMTIME date, time;
	DateTime_GetSystemtime(h_date_from_picker, &date);

	from->year = date.wYear;
	from->month = date.wMonth;
	from->day = date.wDay;

	DateTime_GetSystemtime(h_date_to_picker, &date);

	to->year = date.wYear;
	to->month = date.wMonth;
	to->day = date.wDay;

	DateTime_GetSystemtime(h_time_from_picker, &time);
	from->hours = time.wHour;
	from->minutes = time.wMinute;
	from->seconds = time.wSecond;

	DateTime_GetSystemtime(h_time_to_picker, &time);
	to->hours = time.wHour;
	to->minutes = time.wMinute;
	to->seconds = time.wSecond;

	if (flight_date_comp(from, to) > 0)
	{
		MessageBox(hDlg, L"Date \"from\" should less than the date \"to\".", L"Wrong Condition", MB_OK | MB_ICONERROR);
		goto free_to;
	}

	doc.query.date_from = from;
	doc.query.date_to = to;
	return;

free_to:
	free(to);
free_from:
	free(from);
}

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

static inline void do_query_dest(HWND hDlg)
{
	wchar_t* buf = calloc(64, sizeof(wchar_t));
	if (!buf)
	{
		return;
	}

	CONTROL_TEXT_TO_BUF(IDC_EDIT_DEST, buf, 64);

	int len = lstrlen(buf);
	for (int i = 0; i < len; i++)
	{
		if (!iswalnum(buf[i]))
		{
			MessageBox(hDlg, L"Destination should only contains normal characters.", L"Wrong Condition", MB_OK | MB_ICONERROR);
			goto free_buf;
		}
	}

	doc.query.dest = buf;
	return;

free_buf:
	free(buf);
}

static inline void do_query(HWND hDlg)
{
	doc.query.is_queried = IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE) || IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DEST);

	if (IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE))
	{
		do_query_date(hDlg);
	}
	if (IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DEST))
	{
		do_query_dest(hDlg);
	}

	doc.sort.is_sorted = Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_NAME)) == BST_CHECKED ||
		Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_DATE)) == BST_CHECKED ||
		Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_AVAL)) == BST_CHECKED ||
		Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_SOLD)) == BST_CHECKED;

	if (Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_NAME)) == BST_CHECKED)
	{
		doc.sort.sort_flags |= FLIGHT_QUERY_SORT_NAME;
	}
	else if (Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_DATE)) == BST_CHECKED)
	{
		doc.sort.sort_flags |= FLIGHT_QUERY_SORT_DATE;
	}
	else if (Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_AVAL)) == BST_CHECKED)
	{
		doc.sort.sort_flags |= FLIGHT_QUERY_SORT_TICKET_AVAIL;
	}
	else if (Button_GetState(GetDlgItem(hDlg, IDC_RADIO_SORT_SOLD)) == BST_CHECKED)
	{
		doc.sort.sort_flags |= FLIGHT_QUERY_SORT_TICKET_SOLD;
	}


	load_main_listview();
}

static inline void do_clear(HWND hDlg)
{
	Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_BY_DATE), FALSE);
	Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE_FROM), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
	Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE_TO), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
	Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME_FROM), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
	Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME_TO), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));


	Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_BY_DEST), FALSE);
	Edit_Enable(GetDlgItem(hDlg, IDC_EDIT_DEST), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DEST));

	Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_NO_SORT), TRUE);

	Edit_SetText(GetDlgItem(hDlg, IDC_EDIT_DEST), L"");

	if (doc.query.date_from)
	{
		free(doc.query.date_from);
	}

	if (doc.query.date_to)
	{
		free(doc.query.date_to);
	}

	if (doc.query.dest)
	{
		free(doc.query.dest);
	}

	memset(&doc.query, 0, sizeof(doc.query));
	memset(&doc.sort, 0, sizeof(doc.sort));

	load_main_listview();
}

INT_PTR CALLBACK QueryToolWindowWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_NO_SORT), TRUE);
		return (INT_PTR)TRUE;
	}
	case WM_DESTROY:
	{
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK_BY_DATE:
			Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE_FROM), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
			Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE_TO), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
			Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME_FROM), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
			Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME_TO), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
			break;

		case IDC_CHECK_BY_DEST:
			Edit_Enable(GetDlgItem(hDlg, IDC_EDIT_DEST), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DEST));
			break;

		case IDC_BUTTON_QUERY:
			do_query(hDlg);
			break;

		case IDC_BUTTON_CLEAR:
			do_clear(hDlg);
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
	}
	return (INT_PTR)FALSE;
}


void create_right_panel(HWND hwndParent)
{
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES;

	InitCommonControlsEx(&icex);

	if (!IsWindow(h_query_tool_window))
	{
		h_query_tool_window = CreateDialog(hInst,
			MAKEINTRESOURCE(IDD_DIALOG_QUERY_PANEL),
			hMainWnd,
			(DLGPROC)QueryToolWindowWndProc);

		ShowWindow(h_query_tool_window, SW_SHOW);

		EnableMenuItem(GetSystemMenu(h_query_tool_window, FALSE), SC_CLOSE,
			MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
}


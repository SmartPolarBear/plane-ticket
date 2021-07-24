#include "main.h"

#include <CommCtrl.h>

#include "resource.h"

#include <windowsx.h>

// Right Panel
HWND h_query_tool_window;

static inline void do_query(HWND hDlg)
{
	load_main_listview();
}

static inline void do_clear(HWND hDlg)
{
	Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_BY_DATE), FALSE);
	Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
	Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));

	Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_BY_DEST), FALSE);
	Edit_Enable(GetDlgItem(hDlg, IDC_EDIT_DEST), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DEST));

	Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_NO_SORT), TRUE);
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
			Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
			Button_Enable(GetDlgItem(hDlg, IDC_DATETIMEPICKER_TIME), IsDlgButtonChecked(hDlg, IDC_CHECK_BY_DATE));
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


void create_right_panel(HWND hwndParent, int x, int y, int w, int h)
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


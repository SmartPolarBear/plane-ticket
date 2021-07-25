#include "flight_status_dlg.h"

#include <windowsx.h>

#include "main.h"

static flight_t* the_flight = NULL;

static inline void do_set_flight_status(HWND hDlg)
{
	int abnormals = FFLAG_DELAY | FFLAG_CANCELED;
	the_flight->flags &= ~abnormals;

	if (Button_GetState(GetDlgItem(hDlg, IDC_RADIO_CANCEL)) == BST_CHECKED)
	{
		the_flight->flags |= FFLAG_CANCELED;
	}
	else if (Button_GetState(GetDlgItem(hDlg, IDC_RADIO_DELAYED)) == BST_CHECKED)
	{

		the_flight->flags |= FFLAG_DELAY;
	}

	int ret = save_document(&doc);
	if (ret)
	{
		MessageBox(hDlg, L"Cannot save the change,", L"Failure", MB_OK | MB_ICONERROR);
	}
}

INT_PTR CALLBACK FlightStatusDlgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		if (the_flight->flags & FFLAG_CANCELED)
		{
			Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_CANCEL), TRUE);
		}
		else if (the_flight->flags & FFLAG_DELAY)
		{
			Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_DELAYED), TRUE);
		}
		else
		{
			Button_SetCheck(GetDlgItem(hDlg, IDC_RADIO_NORMAL), TRUE);
		}

		return (INT_PTR)TRUE;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:

			do_set_flight_status(hDlg);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		default:
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


WPARAM show_flight_status_dialog(HWND h_parent, flight_t* f)
{
	the_flight = f;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SET_FLIGHT_STATUS), h_parent, FlightStatusDlgWndProc);
}

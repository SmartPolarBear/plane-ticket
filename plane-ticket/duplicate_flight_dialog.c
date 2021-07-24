#include "duplicate_flight_dialog.h"

#include "main.h"

#include <windowsx.h>

static flight_t* the_flight = NULL;

void do_duplicate_flight(HWND hDlg)
{
	HWND h_date_picker = GetDlgItem(hDlg, IDC_DATETIMEPICKER_DATE);
	SYSTEMTIME date;
	DateTime_GetSystemtime(h_date_picker, &date);

	flight_t* copy = malloc(sizeof(flight_t));
	if (!copy)
	{
		MessageBox(hDlg, L"Insufficient Memory", L"Failure", MB_OK | MB_ICONERROR);
		exit(1);
	}

	memcpy(copy, the_flight, sizeof(flight_t));

	copy->date.year = date.wYear;
	copy->date.month = date.wMonth;
	copy->date.day = date.wDay;

	document_add_flight(&doc, copy);
	free(copy);

	int ret = save_document(&doc);
	if (ret)
	{
		MessageBox(hMainWnd, L"Cannot save flight data!", L"Failure", MB_OK | MB_ICONERROR);
	}
}

INT_PTR CALLBACK DuplicateFlightDlgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			do_duplicate_flight(hDlg);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		}

		break;
	}
	return (INT_PTR)FALSE;
}


DWORD show_duplicate_flight_dialog(HWND h_parent, flight_t* f)
{
	the_flight = f;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DUPLICATE_FLIGHT), h_parent, DuplicateFlightDlgWndProc);
}

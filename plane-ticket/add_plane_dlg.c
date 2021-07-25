#include "framework.h"
#include "add_plane_dlg.h"

#include "resource.h"
#include "helper.h"

#include "document.h"

#include "main.h"

#include <stdlib.h>
#include <windowsx.h>

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



int do_add_plane(HWND hDlg)
{
	flight_t f;
	memset(&f, 0, sizeof(f));

	CONTROL_TEXT_TO_BUF(IDC_EDIT_ID, f.id, 32);

	CONTROL_TEXT_TO_BUF(IDC_EDIT_COMPANY, f.company, 32);

	CONTROL_TEXT_TO_BUF(IDC_EDIT_FROM, f.from, 32);

	CONTROL_TEXT_TO_BUF(IDC_EDIT_TO, f.to, 32);


	wchar_t count_buf[32] = { 0 }, * end = NULL;
	CONTROL_TEXT_TO_BUF(IDC_EDIT_COUNT, count_buf, 32);

	f.sold = 0;
	f.remaining = wcstol(count_buf, &end, 10);

	f.flight_key = hashing(f.id);

	if (IsDlgButtonChecked(hDlg, IDC_CHECK_FIRST))
		f.flags |= FFLAG_CLASS_HAS_FIRST;

	if (IsDlgButtonChecked(hDlg, IDC_CHECK_BUSINESS))
		f.flags |= FFLAG_CLASS_HAS_BUSINESS;

	if (IsDlgButtonChecked(hDlg, IDC_CHECK_ECONOMY))
		f.flags |= FFLAG_CLASS_HAS_ECONOMY;

	if (IsDlgButtonChecked(hDlg, IDC_CHECK_ALLOW_CUSTOM_FOOD))
		f.flags |= FFLAG_ALLOW_CUSTOM_FOOD;

	HWND h_date_picker = GetDlgItem(hDlg, IDC_DATE_PICKER), h_time_picker = GetDlgItem(hDlg, IDC_TIME_PICKER);
	SYSTEMTIME date, time;
	DateTime_GetSystemtime(h_date_picker, &date);
	DateTime_GetSystemtime(h_time_picker, &time);

	f.date.year = date.wYear;
	f.date.month = date.wMonth;
	f.date.day = date.wDay;

	f.date.hours = time.wHour;
	f.date.minutes = time.wMinute;
	f.date.seconds = time.wSecond;

	int ret = document_add_flight(&doc, &f);
	if (ret)
		return FALSE;

	ret = save_document(&doc);
	if (ret)
		return FALSE;

	return TRUE;
}


INT_PTR CALLBACK AddPlaneDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);


	switch (message)
	{
	case WM_INITDIALOG:
		Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_FIRST), TRUE);
		Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_BUSINESS), TRUE);
		Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_ECONOMY), TRUE);
		Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_ALLOW_CUSTOM_FOOD), TRUE);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (!do_add_plane(hDlg))
			{
				MessageBox(hDlg, L"Cannot save new flight information.\nCheck your disk storage.", L"Failure", MB_OK | MB_ICONERROR);
			}

			break;
		default:
			break;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

WPARAM show_add_plane_dlg(HWND h_parent)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_ADDPLANEDLG), hMainWnd, AddPlaneDialog);
}
#include "framework.h"
#include "add_plane_dlg.h"

#include "resource.h"
#include "helper.h"

#include "document.h"

#include "main.h"

#include <stdlib.h>

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


	wchar_t count_buf[32] = { 0 },*end=NULL;
	CONTROL_TEXT_TO_BUF(IDC_EDIT_COUNT, count_buf, 32);

	f.sold = 0;
	f.remaining = wcstol(count_buf, &end, 10);

	f.flight_key = hashing(f.id);

	f.flags |= FFLAG_CLASS_HAS_BUSINESS;
	f.flags |= FFLAG_CLASS_HAS_FIRST;
	f.flags |= FFLAG_CLASS_HAS_ECONOMY;

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

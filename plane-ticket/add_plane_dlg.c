#include "framework.h"
#include "add_plane_dlg.h"

#include "resource.h"

#include "document.h"

extern document_t doc; // in main.cc

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

uint64_t hashing(wchar_t* str)
{
	uint64_t ret = 0;
	for (int i = 0; i < 32; i++)
	{
		if (str[i] != 0)
		{
			ret *= 10;
			ret += (int)(str[i] >= L'0' && str[i] <= '9' ? str[i] - L'0' : str[i] - 'A');
		}
	}
	return ret;
}

BOOL DoOk(HWND hDlg)
{
	flight_t f;

	CONTROL_TEXT_TO_BUF(IDC_EDIT_ID, f.id, 32);

	CONTROL_TEXT_TO_BUF(IDC_EDIT_COMPANY, f.company, 32);

	CONTROL_TEXT_TO_BUF(IDC_EDIT_FROM, f.from, 32);

	CONTROL_TEXT_TO_BUF(IDC_EDIT_FROM, f.to, 32);

	wchar_t count_buf[32] = { 0 };
	CONTROL_TEXT_TO_BUF(IDC_EDIT_COUNT, count_buf, 32);

	f.sold = f.remaining = 0;
	for (int i = 0; i < 32; i++)
	{
		if (count_buf[i] != 0)
		{
			f.remaining *= 10;
			f.remaining += count_buf[i] - L'0';
		}
	}

	f.flight_key = hashing(f.id);


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
			if (!DoOk(hDlg))
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

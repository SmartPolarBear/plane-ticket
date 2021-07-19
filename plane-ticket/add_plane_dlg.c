#include "framework.h"
#include "add_plane_dlg.h"

#include "resource.h"

#include "document.h"

extern DOCUMENT doc;

UINT64 hashing(LPWSTR str)
{
	UINT64 Ret = 0;
	for (int i = 0; i < 32; i++)
	{
		if (str[i] != 0)
		{
			Ret *= 10;
			Ret += (int)str[i];
		}
	}
	return Ret;
}

BOOL DoOk(HWND hDlg)
{
	PLANE p;

	DWORD len = (WORD)SendDlgItemMessage(hDlg,
		IDC_EDIT_COMPANY,
		EM_LINELENGTH,
		(WPARAM)0,
		(LPARAM)0);

	SendDlgItemMessage(hDlg,
		IDC_EDIT_COMPANY,
		EM_GETLINE,
		0,
		p.pszCompany);
	p.pszCompany[len] = 0;

	len = (WORD)SendDlgItemMessage(hDlg,
		IDC_EDIT_ID,
		EM_LINELENGTH,
		(WPARAM)0,
		(LPARAM)0);

	SendDlgItemMessage(hDlg,
		IDC_EDIT_ID,
		EM_GETLINE,
		0,
		p.pszFlightId);
	p.pszFlightId[len] = 0;

	len = (WORD)SendDlgItemMessage(hDlg,
		IDC_EDIT_FROM,
		EM_LINELENGTH,
		(WPARAM)0,
		(LPARAM)0);

	SendDlgItemMessage(hDlg,
		IDC_EDIT_FROM,
		EM_GETLINE,
		0,
		p.pszFrom);
	p.pszFrom[len] = 0;

	len = (WORD)SendDlgItemMessage(hDlg,
		IDC_EDIT_TO,
		EM_LINELENGTH,
		(WPARAM)0,
		(LPARAM)0);

	SendDlgItemMessage(hDlg,
		IDC_EDIT_TO,
		EM_GETLINE,
		0,
		p.pszTo);
	p.pszTo[len] = 0;

	len = (WORD)SendDlgItemMessage(hDlg,
		IDC_EDIT_COUNT,
		EM_LINELENGTH,
		(WPARAM)0,
		(LPARAM)0);

	WCHAR pszCount[32] = { 0 };
	GetDlgItemText(hDlg, IDC_EDIT_COUNT, pszCount, 32);
	pszCount[len] = 0;

	p.iSold = 0;
	p.iRemain = 0;
	for (int i = 0; i < 32; i++)
	{
		if (pszCount[i] != 0)
		{
			p.iRemain *= 10;
			p.iRemain += pszCount[i] - L'0';
		}
	}

	p.iId = hashing(p.pszFlightId);

	if (!AddPlane(&doc, p))
		return FALSE;

	if (!SaveDocument(&doc))
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

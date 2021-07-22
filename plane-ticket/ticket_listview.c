#include "framework.h"

#include "ticket.h"
#include "main.h"

static struct column_def
{
	wchar_t* text;
	int width;
} columns[] =
{
	{L"Flight ID",128},
	{L"Ticket Number",128},
	{L"Passenger",128},
	{L"Passenger ID",256},

};

static inline void set_columns(HWND hDlg)
{
	LVCOLUMN LvCol;
	HWND h_listview = GetDlgItem(hDlg, IDC_LISTTICKET);

	memset(&LvCol, 0, sizeof(LvCol));
	LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	SendMessage(h_listview, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	for (int i = 0; i < sizeof(columns) / sizeof(columns[0]); i++)
	{
		LvCol.cx = columns[i].width;
		LvCol.pszText = columns[i].text;
		SendMessage(h_listview, LVM_INSERTCOLUMN, i, (LPARAM)&LvCol);
	}
}

void create_ticket_listview(HWND hDlg)
{
	set_columns(hDlg);
}
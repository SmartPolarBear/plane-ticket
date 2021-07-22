#define _CRT_SECURE_NO_WARNINGS

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
	{L"Seat",64},
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
static inline void ticket_listview_add(HWND hDlg, ticket_t* t, int idx)
{
	LVITEM LvItem;
	memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members

	HWND h_listview = GetDlgItem(hDlg, IDC_LISTTICKET);

	wchar_t buf[128] = { 0 };

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of text
	LvItem.iItem = idx;          // choose item  
	LvItem.iSubItem = 0;       // Put in first coluom
	LvItem.pszText = target_flight->parent->id; // Text to display (can be from a char variable) (Items)

	SendMessage(h_listview, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send info to the Listview

	swprintf(buf, sizeof(buf) / sizeof(*buf), L"%lld", t->id);
	LvItem.iSubItem = 1;
	LvItem.pszText = buf;
	SendMessage(h_listview, LVM_SETITEM, 0, (LPARAM)&LvItem);
	memset(buf, 0, sizeof(buf));

	LvItem.iSubItem = 2;
	LvItem.pszText = t->seat;
	SendMessage(h_listview, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 3;
	LvItem.pszText = t->owner;
	SendMessage(h_listview, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 4;
	LvItem.pszText = t->owner_id;
	SendMessage(h_listview, LVM_SETITEM, 0, (LPARAM)&LvItem);
}

void load_ticket_listview(HWND hDlg)
{
	if (!target_flight)
	{
		return;
	}

	HWND h_listview = GetDlgItem(hDlg, IDC_LISTTICKET);

	ListView_DeleteAllItems(h_listview);

	document_flight_apply_query(target_flight);

	for (int i = 0; i < target_flight->result_count; i++)
	{
		ticket_listview_add(hDlg, target_flight->result[i], i);
	}
}

void create_ticket_listview(HWND hDlg)
{
	set_columns(hDlg);
}
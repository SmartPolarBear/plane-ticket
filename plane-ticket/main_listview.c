#include "main.h"

// Main List View
HWND hWndMainListView;

LVCOLUMN LvCol;
LVITEM LvItem;

struct column_def
{
	wchar_t* text;
	int width;
}columns[] =
{
	{L"Flight ID",128},
	{L"Company",256},
	{L"From",128},
	{L"To",128},
	{L"Status",128},
};

static inline void set_columns()
{
	memset(&LvCol, 0, sizeof(LvCol));
	LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	SendMessage(hWndMainListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	for (int i = 0; i < sizeof(columns) / sizeof(columns[0]); i++)
	{
		LvCol.cx = columns[i].width;
		LvCol.pszText = columns[i].text;
		SendMessage(hWndMainListView, LVM_INSERTCOLUMN, i, (LPARAM)&LvCol);
	}

}

static inline void main_listview_load(flight_t* f)
{
	memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of txst
	LvItem.iItem = 0;          // choose item  
	LvItem.iSubItem = 0;       // Put in first coluom
	LvItem.pszText = f->id; // Text to display (can be from a char variable) (Items)

	SendMessage(hWndMainListView, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send info to the Listview

	LvItem.iSubItem = 1;
	LvItem.pszText = f->company;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 2;
	LvItem.pszText = f->from;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 3;
	LvItem.pszText = f->to;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);


	LvItem.iSubItem = 4;
	LvItem.pszText = L"TODO";
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);
}


void create_main_listview(HWND hwndParent, int x, int y, int w, int h)
{
	// Create the list-view window in report view with label editing enabled.
	hWndMainListView = CreateWindow(WC_LISTVIEW,
		L"",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
		x,
		y,
		w,
		h,
		hwndParent,
		(HMENU)IDM_MAIN_LISTVIEW,
		hInst,
		NULL);


	if (hWndMainListView == NULL)
	{
		MessageBox(hMainWnd, L"Cannot initialize UI.", L"Failure", MB_OK | MB_ICONERROR);
		exit(1);
	}

	set_columns();
}

void load_main_listview()
{
	if (!doc.flights)
	{
		return;
	}

	ListView_DeleteAllItems(hWndMainListView);

	for (int i = 0; i < doc.header->flight_count; i++)
	{
		main_listview_load(&doc.flights[i]);
	}
}

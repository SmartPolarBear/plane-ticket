#define  _CRT_SECURE_NO_WARNINGS

#include "main.h"

// Main List View
HWND hWndMainListView;
INT64 main_list_view_selected = -1;

LVCOLUMN LvCol;
LVITEM LvItem;

static struct column_def
{
	wchar_t* text;
	int width;
}columns[] =
{
	{L"Flight ID",64},
	{L"Company",128},
	{L"Date",200},
	{L"From",128},
	{L"To",128},
	{L"Status",128},
	{L"Sold",64},
	{L"Available",64},
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

static inline void main_listview_add(flight_t* f, int idx)
{
	wchar_t buf[128] = { 0 };
	memset(buf, 0, sizeof(buf));
	memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of text
	LvItem.iItem = idx;          // choose item  
	LvItem.iSubItem = 0;       // Put in first coluom
	LvItem.pszText = f->id; // Text to display (can be from a char variable) (Items)

	SendMessage(hWndMainListView, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send info to the Listview

	LvItem.iSubItem = 1;
	LvItem.pszText = f->company;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	memset(buf, 0, sizeof(buf));
	swprintf(buf, sizeof(buf) / sizeof(buf[0]), L"%lld/%lld/%lld %lld:%lld:%lld", f->date.year, f->date.month, f->date.day,
		f->date.hours, f->date.minutes, f->date.seconds);
	LvItem.iSubItem = 2;
	LvItem.pszText = buf;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 3;
	LvItem.pszText = f->from;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 4;
	LvItem.pszText = f->to;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	LvItem.iSubItem = 5;
	if (f->flags & FFLAG_CANCELED)
	{
		LvItem.pszText = L"Cancelled";
	}
	else if (f->flags & FFLAG_DELAY)
	{
		LvItem.pszText = L"Delay";
	}
	else
	{
		LvItem.pszText = L"Normal";
	}
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	memset(buf, 0, sizeof(buf));
	wsprintf(buf, L"%ld", f->sold);
	LvItem.iSubItem = 6;
	LvItem.pszText = buf;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);

	memset(buf, 0, sizeof(buf));
	wsprintf(buf, L"%ld", f->remaining);
	LvItem.iSubItem = 7;
	LvItem.pszText = buf;
	SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem);
}


void create_main_listview(HWND hwndParent, int x, int y, int w, int h)
{
	// Create the list-view window in report view with label editing enabled.
	hWndMainListView = CreateWindow(WC_LISTVIEW,
		L"",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS| LVS_SINGLESEL,
		x,
		y,
		w,
		h,
		hwndParent,
		(HMENU)IDC_MAINLISTVIEW,
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

	document_apply_query(&doc);
	for (int i = 0; i < doc.result_count; i++)
	{
		main_listview_add(doc.result[i], i);
	}
}

void main_list_view_delete_selected_item()
{
	if (main_list_view_selected < 0)return;

	wchar_t* notify_text = calloc(64, sizeof(wchar_t));
	if (!notify_text)
	{
		MessageBox(hMainWnd, L"Insufficient memory!", L"Failure", MB_OK | MB_ICONERROR);
		exit(1);
	}

	flight_t* selected = doc.result[main_list_view_selected];


	wsprintf(notify_text, L"Are you sure to delete flight %s?", selected->id);
	int msg_id = MessageBox(hMainWnd, notify_text, L"Confirm", MB_ICONQUESTION | MB_YESNO);
	if (msg_id == IDNO)
	{
		free(notify_text);
		return;
	}
	free(notify_text);


	document_remove_flight(selected);
	save_document(&doc);

	ListView_DeleteItem(hWndMainListView, main_list_view_selected);

	main_list_view_selected = -1;

}

void main_listview_resize()
{
	RECT rcClient;
	GetClientRect(hMainWnd, &rcClient);

	SetWindowPos(hWndMainListView, HWND_TOP, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 0);
}
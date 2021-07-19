// plane-ticket.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "plane-ticket.h"

#include "document.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
DOCUMENT doc;

// Main List View
HWND hWndMainListView;
LVCOLUMN LvCol;
LVITEM LvItem;

// Right Panel
HWND hWndRightPanel;
const UINT32 iRightPanelWidth = 200, iRightPanelPaddingLeft = 4, iRightPanelPaddingRight;
const UINT32 iInnerButtonWidth = 180, iInnerButtonHeight = 40, iInnerButtonPadding = 10;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	INITCOMMONCONTROLSEX icc;

	// Initialize common controls.
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PLANETICKET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANETICKET));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLANETICKET));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLANETICKET);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

HWND CreateRightPanel(HWND hwndParent)
{
	RECT rcClient;                       // The parent window's client area.
	GetClientRect(hwndParent, &rcClient);

	HWND hWndGroup = CreateWindow(WC_BUTTON, L"Plane Action",
		WS_CHILD | WS_VISIBLE | WS_GROUP | BS_GROUPBOX,
		rcClient.right - iRightPanelWidth - iRightPanelPaddingRight,
		0,
		iRightPanelWidth,
		rcClient.bottom - rcClient.top,

		hwndParent, NULL, hInst, 0);

	return hWndGroup;
}

void LoadRightPanel()
{
	RECT rcClient;
	GetClientRect(hWndRightPanel, &rcClient);

	HWND hWndButtonAdd = CreateWindow(WC_BUTTON, L"New Plane",
		WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON,
		rcClient.left + iInnerButtonPadding,
		rcClient.top + iInnerButtonPadding + 20,
		iInnerButtonWidth,
		iInnerButtonHeight,
		hWndRightPanel, NULL, hInst, 0);
}

HWND CreateMainListView(HWND hwndParent)
{

	RECT rcClient;                       // The parent window's client area.
	GetClientRect(hwndParent, &rcClient);

	// Create the list-view window in report view with label editing enabled.
	HWND hWndListView = CreateWindow(WC_LISTVIEW,
		L"",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
		0,
		0,
		rcClient.right - rcClient.left - iRightPanelWidth - iRightPanelPaddingLeft - iRightPanelPaddingRight,
		rcClient.bottom - rcClient.top,
		hwndParent,
		(HMENU)IDM_MAIN_LISTVIEW,
		hInst,
		NULL);

	return hWndListView;
}

BOOL LoadListView()
{
	memset(&LvCol, 0, sizeof(LvCol));
	LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	LvCol.cx = 128;

	SendMessage(hWndMainListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	UINT16* Cols[] = { L"Flight ID" , L"Company", L"From", L"To", L"Status" };
	for (int i = 0; i < sizeof(Cols) / sizeof(Cols[0]); i++)
	{
		LvCol.pszText = Cols[i];
		SendMessage(hWndMainListView, LVM_INSERTCOLUMN, i, (LPARAM)&LvCol);
	}

	memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members

//  Setting properties Of members:

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of test
	LvItem.iItem = 0;          // choose item  
	LvItem.iSubItem = 0;       // Put in first coluom
	LvItem.pszText = L"Item 0"; // Text to display (can be from a char variable) (Items)

	SendMessage(hWndMainListView, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send info to the Listview



	for (int i = 1; i <= 5; i++) // Add SubItems in a loop
	{
		LvItem.iSubItem = i;
		LvItem.pszText = L"Item";
		SendMessage(hWndMainListView, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter text to SubItems
	}
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	hWndRightPanel = CreateRightPanel(hWnd);
	if (hWndRightPanel == NULL)
	{
		return FALSE;
	}
	LoadRightPanel();

	hWndMainListView = CreateMainListView(hWnd);
	if (hWndMainListView == NULL)
	{
		return FALSE;
	}
	LoadListView();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	InitDocument(&doc);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_NEW:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

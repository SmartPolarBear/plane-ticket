// plane-ticket.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "main.h"

#include "document.h"

#include "about_dlg.h"

#include "add_plane_dlg.h"

#include "ticket_find_dlg.h"
#include "ticket_booking_dlg.h"
#include "flight_status_dlg.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hMainWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

document_t doc;


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

	hMainWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	make_main_layout();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	int ret = load_document(&doc);
	if (ret)
	{
		MessageBox(hWnd, L"Cannot initialize data.", L"Failure", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	load_main_listview();

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
		case IDM_FLIGHT_VIEW_TICKETS:
			show_ticket_dialog();
			break;
		case IDM_FLIGHT_ADD:
			if (show_add_plane_dialog(hMainWnd)==IDOK)
			{
				load_main_listview();
			}
			break;
		case IDM_FLIGHT_DELETE:
			main_list_view_delete_selected_item();
			break;
		case IDM_FLIGHT_BOOK:
		{
			flight_info_t* info = malloc(sizeof(flight_info_t));

			if (!info)
			{
				free(info);

				MessageBox(hMainWnd, L"Insufficient memory!", L"Failure", MB_OK | MB_ICONERROR);
				exit(1);
			}

			int ret = document_get_flight_info(doc.result[main_list_view_selected], info);
			if (ret)
			{
				free(info);

				MessageBox(hMainWnd, L"Cannot load flight data!", L"Failure", MB_OK | MB_ICONERROR);
				break;
			}

			if (show_booking_dialog(hMainWnd, info) == IDOK)
			{
				load_main_listview();
			}

			free(info);

			break;
		}
		case ID_SELECTED_FINDSEATS:
		{
			flight_info_t* info = malloc(sizeof(flight_info_t));

			if (!info)
			{
				free(info);
				MessageBox(hMainWnd, L"Insufficient memory!", L"Failure", MB_OK | MB_ICONERROR);
				exit(1);
			}

			int ret = document_get_flight_info(doc.result[main_list_view_selected], info);
			if (ret)
			{
				free(info);
				MessageBox(hMainWnd, L"Cannot load flight data!", L"Failure", MB_OK | MB_ICONERROR);
				break;
			}

			ticket_find_dialog_targeted(hMainWnd, info);

			free(info);

			break;
		}
		case IDM_ABOUT:
			show_about_dialog(hMainWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FLIGHT_DELETE_CANCELLED:
		{
			DWORD confirm = MessageBox(hMainWnd, L"Are you sure to delete all cancelled flights?", L"Comfirm", MB_YESNO | MB_ICONQUESTION);
			
			if (confirm != IDYES)
			{
				break;
			}

			for (int i = 0; i < doc.header->flight_count; i++)
			{
				if ((doc.flights[i].flags & FFLAG_DELETE) == 0 && (doc.flights[i].flags & FFLAG_CANCELED) != 0)
				{
					document_remove_flight(&doc.flights[i]);
				}
			}

			int ret = save_document(&doc);
			if (ret)
			{
				MessageBox(hMainWnd, L"Cannot save flight data!", L"Failure", MB_OK | MB_ICONERROR);
				break;
			}

			load_main_listview();
			break;
		}
		case ID_SELECTED_DUPLICATE:
		{
			main_list_view_duplicate_selected_item();
			break;
		}
		case ID_SELECTED_SETSTATUS:
			if (show_flight_status_dialog(hMainWnd, doc.result[main_list_view_selected]) == IDOK)
			{
				load_main_listview();
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
	{
		update_main_layout();
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		// free the document
		int ret = save_document(&doc);
		if (ret)
		{
			MessageBox(hWnd, L"Cannot save data.", L"Failure", MB_OK | MB_ICONERROR);
		}

		destroy_document(&doc);

		PostQuitMessage(0);
		break;
	case WM_RBUTTONUP:
		main_list_view_selected = SendMessage(hWndMainListView, LVM_GETNEXTITEM,
			-1, LVNI_FOCUSED | LVNI_SELECTED);
		break;
	case WM_NOTIFY:
		switch (LOWORD(wParam))
		{
		case IDC_MAINLISTVIEW:
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_CLICK:
				main_list_view_selected = SendMessage(hWndMainListView, LVM_GETNEXTITEM,
					-1, LVNI_FOCUSED | LVNI_SELECTED);
				break;

			case NM_DBLCLK:
				main_list_view_selected = SendMessage(hWndMainListView, LVM_GETNEXTITEM,
					-1, LVNI_FOCUSED | LVNI_SELECTED);

				if (main_list_view_selected < 0)
					break;

				show_ticket_dialog();

				break;

			case NM_RCLICK:
				main_list_view_selected = SendMessage(hWndMainListView, LVM_GETNEXTITEM,
					-1, LVNI_FOCUSED | LVNI_SELECTED);

				HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDM_MAIN_LISTVIEW_POPUP));

				HMENU hPopupMenu;

				if (main_list_view_selected < 0) // no item selected
				{
					hPopupMenu = GetSubMenu(hMenu, 1);
				}
				else
				{
					hPopupMenu = GetSubMenu(hMenu, 0);
				}

				POINT pt;
				GetCursorPos(&pt);


				SetMenuDefaultItem(hPopupMenu, -1, TRUE);

				SetForegroundWindow(hWnd);
				TrackPopupMenu(hPopupMenu,
					TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
				SetForegroundWindow(hWnd);

				DestroyMenu(hPopupMenu);
				DestroyMenu(hMenu);

				break;
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


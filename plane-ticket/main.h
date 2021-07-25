#pragma once

#include "resource.h"
#include "framework.h"

#include "document.h"

#include "main_layout.h"
#include "control_id.h"

extern HWND hWndMainListView;
extern INT64 main_list_view_selected;


extern HWND hMainWnd;
extern HINSTANCE hInst;
extern HWND hStatusBar;

extern HWND h_query_tool_window;

extern document_t doc;

void make_main_layout();
void update_main_layout();

void create_main_listview(HWND hwndParent, int x, int y, int w, int h);
void main_listview_resize();
void load_main_listview();
void main_list_view_delete_selected_item();
void main_list_view_duplicate_selected_item();

void create_right_panel(HWND hwndParent);

void create_status_bar();
void status_bar_resize();

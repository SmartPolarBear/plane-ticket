#pragma once

#include "resource.h"
#include "framework.h"

#include "document.h"

#include "main_layout.h"
#include "control_id.h"

extern HWND hWndMainListView;
extern HWND hWndRightPanel;

extern HWND hMainWnd;
extern HINSTANCE hInst;

extern document_t doc;

void make_main_layout();

void create_main_listview(HWND hwndParent, int x, int y, int w, int h);
void load_main_listview();

void create_right_panel(HWND hwndParent, int x, int y, int w, int h);

void show_add_flight_dialog();
void show_about_dialog();
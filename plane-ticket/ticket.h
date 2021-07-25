#pragma once
#include "framework.h"
#include "document.h"

extern struct flight_info* target_flight;

void show_ticket_dialog();

void create_ticket_listview(HWND parent);

void load_ticket_listview(HWND hDlg);
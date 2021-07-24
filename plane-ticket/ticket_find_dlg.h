#pragma once

#include "framework.h"

#include "document.h"

WPARAM ticket_find_dialog(HWND hDlg);
WPARAM ticket_find_dialog_targeted(HWND hDlg,flight_info_t *flight);
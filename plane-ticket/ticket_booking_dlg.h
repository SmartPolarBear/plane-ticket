#pragma once
#include "framework.h"

#include "document.h"

WPARAM show_booking_dialog(HWND hDlg,flight_info_t *target);
WPARAM show_booking_dialog_target_seat(HWND hDlg, flight_info_t* target,wchar_t *seat,int flags);


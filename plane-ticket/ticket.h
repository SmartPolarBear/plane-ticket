#pragma once

#include "framework.h"

typedef struct tagTICKET
{
	int iPlaneId;
	WCHAR pszTicketNumber[32];
	WCHAR pszOwnerName[32];
	WCHAR pszOwnerId[32];
}TICKET, * LPTICKET;
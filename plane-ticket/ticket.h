#pragma once

#include "framework.h"

typedef struct tagTICKET
{
	int iPlaneId;
	LPWSTR pszTicketNumber;
	LPWSTR pszOwnerName;
	LPWSTR pszOwnerId;
}TICKET,*LPTICKET;
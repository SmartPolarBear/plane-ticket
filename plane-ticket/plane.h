#pragma once
#include "framework.h"

typedef struct tagPLANE
{
	UINT64 iId;
	WCHAR pszCompany[32];
	WCHAR pszFrom[32];
	WCHAR pszTo[32];

	UINT64 iRemain, iSold;
}PLANE, * LPPLANE;
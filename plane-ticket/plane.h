#pragma once
#include "framework.h"

typedef struct tagPLANE
{
	UINT64 iId;
	LPWSTR pszCompany;
	LPWSTR pszFrom;
	LPWSTR pszTo;

	UINT64 iRemain, iSold;
}PLANE,*LPPLANE;
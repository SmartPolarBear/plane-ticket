#include "document.h"

#include "helper.h"

LPCWSTR pszDocumentName = L"main_db";

static inline BOOL InitPlaneHeader(LPPLANEHEADER lpPlaneHeader)
{
	memset(lpPlaneHeader, 0, sizeof(PLANEHEADER));

	if (FileExists(pszDocumentName))
	{
		HANDLE hFile = CreateFile(pszDocumentName, GENERIC_ALL, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, 0, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwBytesRead = 0;
			if (!ReadFile(hFile, (LPVOID)lpPlaneHeader, sizeof(PLANEHEADER), &dwBytesRead, NULL))
			{
				CloseHandle(hFile);
				return FALSE;
			}
			CloseHandle(hFile);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		lpPlaneHeader->iTotal = 0;

		HANDLE hFile = CreateFile(pszDocumentName, GENERIC_ALL, FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, 0, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwBytesWrite = 0;
			if (!WriteFile(hFile, (LPVOID)lpPlaneHeader,
				sizeof(PLANEHEADER) + lpPlaneHeader->iTotal * sizeof(PLANEINFO),
				&dwBytesWrite, NULL))
			{
				CloseHandle(hFile);
				return FALSE;
			}

			CloseHandle(hFile);

		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

static inline BOOL LoadTicket(LPDOCUMENT lpDoc, LPPLANEINFO lpPlane, LPTICKETHEADER lpTicketHeader)
{
	UINT64 newSize = sizeof(TICKETHEADER) + lpTicketHeader->iTotal * sizeof(TICKETINFO);
	LPVOID res = realloc(lpDoc->lpPlaneHeader, newSize);
	if (!res)
	{
		return FALSE;
	}
	lpDoc->lpPlaneHeader = res;


	HANDLE hFile = CreateFile(lpPlane->szConfigFileName, GENERIC_ALL, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesRead = 0;
		if (!ReadFile(hFile, (LPVOID)res, newSize, &dwBytesRead, NULL))
		{
			CloseHandle(hFile);
			return FALSE;
		}
		CloseHandle(hFile);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

static inline BOOL LoadPlane(LPDOCUMENT lpDoc, LPPLANEINFO lpPlane)
{
	++lpDoc->iTicketHeaderCount;
	LPTICKETHEADER* res = realloc(lpDoc->lpTicketHeaders,
		lpDoc->iTicketHeaderCount * sizeof(LPTICKETHEADER));

	if (!res)
		return FALSE;

	lpDoc->lpTicketHeaders = res;

	LPTICKETHEADER lpNewHeader = res[lpDoc->iTicketHeaderCount - 1] = malloc(sizeof(TICKETHEADER));
	if (!lpNewHeader)
	{
		return FALSE;
	}

	memset(lpNewHeader, 0, sizeof(TICKETHEADER));


	HANDLE hFile = CreateFile(lpPlane->szConfigFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_ALWAYS, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, (LPVOID)lpNewHeader, sizeof(TICKETHEADER), &dwBytesRead, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);

	return LoadTicket(lpDoc, lpPlane, lpNewHeader);
}

static inline BOOL SavePlane(LPDOCUMENT lpDoc, LPPLANEINFO lpPlane)
{
	HANDLE hFile = CreateFile(lpPlane->szConfigFileName, GENERIC_ALL, FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		for (int i = 0; i < lpDoc->iTicketHeaderCount; i++)
		{
			if (lpDoc->lpTicketHeaders[i]->iPlaneId == lpPlane->Plane.iId)
			{
				DWORD dwBytesWrite = 0;
				if (!WriteFile(hFile, (LPVOID)&lpDoc->lpTicketHeaders[i],
					sizeof(TICKETHEADER) + lpDoc->lpTicketHeaders[i]->iTotal * sizeof(TICKETINFO),
					&dwBytesWrite, NULL))
				{
					CloseHandle(hFile);
					return FALSE;
				}

				CloseHandle(hFile);
				break;
			}
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


static inline BOOL ApplyQuery(LPDOCUMENT lpDoc, LPPLANEINFO lpPlaneInfo)
{
	if (!lpDoc->Query.bHasQuery)return TRUE;
	switch (lpDoc->Query.Scoop)
	{
	default:
		return FALSE;
	}
}

BOOL InitDocument(LPDOCUMENT lpDoc)
{
	memset(lpDoc, 0, sizeof(DOCUMENT));
	lpDoc->lpPlaneHeader = malloc(sizeof(PLANEHEADER));

	BOOL ret = InitPlaneHeader(lpDoc->lpPlaneHeader);
	if (!ret)return ret;

	return 0;
}

BOOL LoadDocument(LPDOCUMENT lpDoc)
{

	HANDLE hFile = CreateFile(pszDocumentName, GENERIC_READ, FILE_SHARE_READ,
		NULL, CREATE_NEW, 0, NULL);
	if (GetLastError() == ERROR_FILE_EXISTS)
	{
		hFile = CreateFile(pszDocumentName, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, 0, NULL);
	}

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	UINT64 newSize = sizeof(PLANEHEADER);
	LPVOID res = realloc(lpDoc->lpPlaneHeader, newSize);
	if (!res)
	{
		return FALSE;
	}

	lpDoc->lpPlaneHeader = res;

	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, (LPVOID)res, sizeof(PLANEHEADER), &dwBytesRead, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	newSize = sizeof(PLANEHEADER) + lpDoc->lpPlaneHeader->iTotal * sizeof(PLANEINFO);
	res = realloc(lpDoc->lpPlaneHeader, newSize);
	if (!res)
	{
		return FALSE;
	}
	lpDoc->lpPlaneHeader = res;

	SetFilePointer(hFile, 0, 0, FILE_BEGIN);
	if (!ReadFile(hFile, (LPVOID)res, newSize, &dwBytesRead, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (newSize != dwBytesRead)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);


	for (int i = 0; i < lpDoc->lpPlaneHeader->iTotal; i++)
	{
		if (lpDoc->lpPlaneHeader->Planes[i].bDeleted)
			continue;
		if (lpDoc->lpPlaneHeader->Planes[i].bEnd)
			break;

		if (!LoadPlane(lpDoc, &lpDoc->lpPlaneHeader->Planes[i]))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL SaveDocument(LPDOCUMENT lpDoc)
{
	for (int i = 0; i < lpDoc->lpPlaneHeader->iTotal; i++)
	{
		if (lpDoc->lpPlaneHeader->Planes[i].bDeleted)continue;
		if (lpDoc->lpPlaneHeader->Planes[i].bEnd)break;

		if (!SavePlane(lpDoc, &lpDoc->lpPlaneHeader->Planes[i]))
		{
			return FALSE;
		}
	}

	HANDLE hFile = CreateFile(pszDocumentName, GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesWrite = 0;
		if (!WriteFile(hFile, (LPVOID)lpDoc->lpPlaneHeader,
			sizeof(PLANEHEADER) + lpDoc->lpPlaneHeader->iTotal * sizeof(PLANEINFO),
			&dwBytesWrite, NULL))
		{
			CloseHandle(hFile);
			return FALSE;
		}

		CloseHandle(hFile);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void DestoryDocument(LPDOCUMENT lpDoc)
{
	for (int i = 0; i < lpDoc->iTicketHeaderCount; i++)
	{
		free(lpDoc->lpTicketHeaders[i]);
	}
	free(lpDoc->lpTicketHeaders);
	free(lpDoc->lpPlaneHeader);
}

BOOL DocumentLoadPlanes(LPDOCUMENT lpDoc, LPPLANE lpBuf, UINT64* iCount)
{
	UINT64 iValidCount = 0;
	for (int i = 0; i < lpDoc->lpPlaneHeader->iTotal; i++)
	{
		if (lpDoc->lpPlaneHeader->Planes[i].bDeleted)continue;
		if (lpDoc->lpPlaneHeader->Planes[i].bEnd)break;

		iValidCount++;
	}

	if (!lpBuf)
	{
		*iCount = iValidCount;
		return TRUE;
	}

	UINT64 idx = 0;
	for (int i = 0; i < lpDoc->lpPlaneHeader->iTotal; i++)
	{
		if (lpDoc->lpPlaneHeader->Planes[i].bDeleted)continue;
		if (lpDoc->lpPlaneHeader->Planes[i].bEnd)break;

		if (ApplyQuery(lpDoc, &lpDoc->lpPlaneHeader->Planes[i]))
		{
			memcpy(lpBuf + idx, &lpDoc->lpPlaneHeader->Planes[i].Plane, sizeof(PLANE));
		}
	}

	return TRUE;
}

BOOL AddPlane(LPDOCUMENT lpDoc, PLANE Plane)
{
	lpDoc->lpPlaneHeader->iTotal++;
	UINT64 newSize = sizeof(PLANEHEADER) + (lpDoc->lpPlaneHeader->iTotal) * sizeof(PLANEINFO);
	LPPLANEHEADER newHeader = realloc(lpDoc->lpPlaneHeader, newSize);
	if (!newHeader)return FALSE;

	lpDoc->lpPlaneHeader = newHeader;

	memset(&newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1], 0,
		sizeof(newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1]));


	newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1].Plane = Plane;
	newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1].bDeleted = FALSE;
	newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1].bEnd = FALSE;

	memcpy(&newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1].szConfigFileName,
		newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1].Plane.pszFlightId,
		sizeof(newHeader->Planes[lpDoc->lpPlaneHeader->iTotal - 1].Plane.pszFlightId));

	return TRUE;
}

BOOL RemovePlane(LPDOCUMENT lpDoc, PLANE Plane)
{
	for (int i = 0; i < lpDoc->lpPlaneHeader->iTotal; i++)
	{
		if (lpDoc->lpPlaneHeader->Planes[i].bDeleted)continue;
		if (lpDoc->lpPlaneHeader->Planes[i].bEnd)break;

		if (lpDoc->lpPlaneHeader->Planes[i].Plane.iId == Plane.iId)
		{
			lpDoc->lpPlaneHeader->Planes[i].bDeleted = TRUE;
		}
	}
	return TRUE;
}

BOOL PlaneBookTicket(LPDOCUMENT lpDoc, PLANE PlaneInfo, TICKET ticket)
{
	return 0;
}

BOOL PlaneRefoundTicket(LPDOCUMENT lpDoc, PLANE PlaneInfo, TICKET ticket)
{
	return 0;
}



void SetQuery(LPDOCUMENT lpDoc, BOOL hasQuery, LPVOID pQueryData, SCOOP sc)
{
	ClearQuery(lpDoc);

	lpDoc->Query.bHasQuery = hasQuery;
	lpDoc->Query.pQueryData = pQueryData;
	lpDoc->Query.Scoop = sc;
}

void ClearQuery(LPDOCUMENT lpDoc)
{
	memset(&lpDoc->Query, 0, sizeof(lpDoc->Query));
}

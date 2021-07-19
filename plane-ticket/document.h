#pragma once

#include "framework.h"

#include "plane.h"
#include "ticket.h"

typedef struct tagTICKETINFO
{
	TICKET Ticket;
	BOOL bDeleted;
	BOOL bEnd;
}TICKETINFO, * LPTICKETINFO;

typedef struct tagTICKETHEADER
{
	UINT64 iTotal;
	UINT64 iPlaneId;
	TICKETINFO Tickets[0];
}TICKETHEADER, * LPTICKETHEADER;

typedef struct tagPLANEINFO
{
	WCHAR szConfigFileName[256];
	PLANE Plane;
	BOOL bDeleted;
	BOOL bEnd;
}PLANEINFO, * LPPLANEINFO;

typedef struct tagPLANEHEADER
{
	UINT64 iTotal;
	PLANEINFO Planes[0];
}PLANEHEADER, * LPPLANEHEADER;

typedef enum tagScope
{
	SC_INVALID, SC_TO, SC_TIME, SC_POS, SC_CONTINIOUS
}SCOOP;

typedef struct tagDocument
{
	LPPLANEHEADER lpPlaneHeader;

	LPTICKETHEADER* lpTicketHeaders;
	UINT64 iTicketHeaderCount;

	struct
	{
		LPPLANEINFO lpPlaneQueryResult;
		UINT64 iPlaneQueryResult;
	} QueryResult;

	struct {
		BOOL bHasQuery;
		LPVOID pQueryData;
		SCOOP Scoop;
	} Query;
}DOCUMENT, * LPDOCUMENT;

BOOL InitDocument(LPDOCUMENT lpDoc);
BOOL LoadDocument(LPDOCUMENT lpDoc);
BOOL SaveDocument(LPDOCUMENT lpDoc);
void DestoryDocument(LPDOCUMENT lpDoc);

void SetQuery(LPDOCUMENT lpDoc, BOOL hasQuery, LPVOID pQueryData, SCOOP sc);
void ClearQuery(LPDOCUMENT lpDoc);
BOOL DocumentLoadPlanes(LPDOCUMENT lpDoc, LPPLANE lpBuf, UINT64* iCount);

BOOL AddPlane(LPDOCUMENT lpDoc, PLANE Plane);
BOOL RemovePlane(LPDOCUMENT lpDoc, PLANE Plane);
BOOL PlaneBookTicket(LPDOCUMENT lpDoc, PLANE PlaneInfo, TICKET ticket);
BOOL PlaneRefoundTicket(LPDOCUMENT lpDoc, PLANE PlaneInfo, TICKET ticket);


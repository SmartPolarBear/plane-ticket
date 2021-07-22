#pragma once

#include "framework.h"

#include "ticket.h"

#include <stdint.h>

enum
{
	FFLAG_DELETE = 0b1
};

enum
{
	TFLAG_DELETE = 0b1
};

typedef struct flight
{
	wchar_t id[32];
	wchar_t company[32];
	wchar_t from[32];
	wchar_t to[32];
	uint64_t sold;
	uint64_t remaining;

	uint64_t flight_key;
	uint64_t flags;
}flight_t;

typedef struct ticket
{
	wchar_t ticket_id[32];
	wchar_t owner[32];
	wchar_t owner_id[32];

	uint64_t flight_key;
	uint64_t flags;
}ticket_t;

typedef struct flight_header
{
	uint64_t ticket_count;
}flight_header_t;

typedef struct flight_info
{
	flight_header_t* header;
	ticket_t* tickets;

	struct
	{
		int is_queried;
	}query;

	struct
	{
		int is_sorted;
	}sort;

	ticket_t* result;
	size_t result_count;
}flight_info_t;



typedef struct document_header
{
	uint64_t flight_count;
}document_header_t;

typedef struct document
{
	document_header_t* header;
	flight_t* flights;

	struct
	{
		int is_queried;
	}query;

	struct
	{
		int is_sorted;
	}sort;

	flight_t* result;
	size_t result_count;
}document_t;


int load_document(document_t* doc);
int save_document(document_t* doc);

int document_apply_query(document_t* doc);

int document_add_flight(document_t* doc, flight_t* flight);
void document_remove_flight(document_t* doc, flight_t* flight);

int document_get_flight_info(flight_t* flight, flight_info_t* out_info);
void document_destroy_flight_info(flight_info_t* flight);

void destroy_document(document_t* doc);
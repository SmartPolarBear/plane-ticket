#pragma once

#include "framework.h"

#include "ticket.h"

#include <stdint.h>

enum
{
	FFLAG_DELETE = 0b1,

	FFLAG_DELAY = 0b10,
	FFLAG_CANCELED = 0b100,

	FFLAG_CLASS_HAS_FIRST = 0b1000,
	FFLAG_CLASS_HAS_BUSINESS = 0b10000,
	FFLAG_CLASS_HAS_ECONOMY = 0b100000,

	FFLAG_ALLOW_CUSTOM_FOOD = 0b1000000
};

enum
{
	FLIGHT_QUERY_NO_SORT = 0b1,
	FLIGHT_QUERY_SORT_DATE = 0b10,
	FLIGHT_QUERY_SORT_NAME = 0b100,
	FLIGHT_QUERY_SORT_TICKET_AVAIL = 0b1000,
	FLIGHT_QUERY_SORT_TICKET_SOLD = 0b10000,

};

enum
{
	TFLAG_DELETE = 0b1,
	TFLAG_UNACCOMPANIED_CHILDREN = 0b10,
	TFLAG_MEDICAL_NEEDS = 0b100,

	TFLAG_CLASS_FIRST = 0b1000,
	TFLAG_CLASS_ECONOMY = 0b10000,
	TFLAG_CLASS_BUSINESS = 0b1000000,

	TFLAG_FOOD_MUSLIM = 0b10000000,
	TFLAG_FOOD_VEGETARIAN = 0b100000000
};

enum
{
	SEAT_A = (1 << 0),
	SAET_B = (1 << 1),
	SEAT_C = (1 << 2),
	SEAT_H = (1 << 3),
	SEAT_J = (1 << 4),
	SEAT_K = (1 << 5),
};

typedef struct flight_date
{
	uint64_t year, month, day, hours, minutes, seconds;
}flight_date_t;

int flight_date_comp(const flight_date_t* f1, const flight_date_t* f2);

typedef struct flight
{
	wchar_t id[32];

	wchar_t company[32];
	wchar_t from[32];
	wchar_t to[32];

	uint64_t sold;
	uint64_t remaining;

	flight_date_t date;

	uint8_t ticket_bitmap[256];

	uint64_t flight_key;
	uint64_t flags;
}flight_t;

typedef struct ticket
{
	uint64_t id;
	wchar_t seat[8];
	wchar_t owner[32];
	wchar_t owner_id[32];
	uint64_t owner_phone;
	wchar_t notes[256];

	uint64_t flight_key;
	uint64_t ticket_key;

	uint64_t flags;
}ticket_t;

typedef struct flight_header
{
	uint64_t ticket_count;
}flight_header_t;

typedef struct flight_info
{
	flight_t* parent;

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

	ticket_t** result;
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
		flight_date_t* date_from, * date_to;
		wchar_t* dest;
		int is_queried;
	}query;

	struct
	{
		uint32_t sort_flags;
		int is_sorted;
	}sort;


	flight_t** result;
	size_t result_count;
}document_t;


int load_document(document_t* doc);
int save_document(document_t* doc);

int document_apply_query(document_t* doc);

int document_add_flight(document_t* doc, flight_t* flight);
void document_remove_flight(flight_t* flight);

void destroy_document(document_t* doc);

int document_get_flight_info(flight_t* flight, flight_info_t* out_info);
int documeent_flight_info_save(document_t* doc, flight_info_t* out_info);

int document_flight_apply_query(flight_info_t* info);

int document_flight_get_rows(flight_info_t* f);

int  document_flight_book_ticket(flight_info_t* info, ticket_t* ticket);
int document_flight_refound_ticket(flight_info_t* info, ticket_t* ticket);

void document_destroy_flight_info(flight_info_t* info);





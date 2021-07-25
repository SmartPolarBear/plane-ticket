#pragma once

#include "framework.h"

#include "ticket.h"

#include <stdint.h>

/// @brief The bits for flight_t's flags field
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

/// @brief flags for document's sort
enum
{
	FLIGHT_QUERY_NO_SORT = 0b1,
	FLIGHT_QUERY_SORT_DATE = 0b10,
	FLIGHT_QUERY_SORT_NAME = 0b100,
	FLIGHT_QUERY_SORT_TICKET_AVAIL = 0b1000,
	FLIGHT_QUERY_SORT_TICKET_SOLD = 0b10000,

};

/// @brief flags for ticket
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

/// @brief bitwise definition for flight's seat bitmap
enum
{
	SEAT_A = (1 << 0),
	SAET_B = (1 << 1),
	SEAT_C = (1 << 2),
	SEAT_H = (1 << 3),
	SEAT_J = (1 << 4),
	SEAT_K = (1 << 5),
};

/// @brief date time field for flight
typedef struct flight_date
{
	uint64_t year, month, day, hours, minutes, seconds;
}flight_date_t;

/// @brief Compare two flight_date_t
/// @param f1 
/// @param f2 
/// @return less than 0 if f1<f2 and greater if f1>f2
int flight_date_comp(const flight_date_t* f1, const flight_date_t* f2);

/// @brief flight information to be saved in disk
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

/// @brief ticket information to save in the disk
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

/// @brief file header for ticket's configuration file
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


/// @brief header for the main database
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

/// @brief load document from disk
/// @param doc the document object
/// @return 0 if succeeded, error code otherwise
int load_document(document_t* doc);

/// @brief save document to disk
/// @param doc 
/// @return 0 if succeeded, error code otherwise
int save_document(document_t* doc);

/// @brief apply the query settings in doc
/// @param doc 
/// @return 0 if succeeded, error code otherwise
int document_apply_query(document_t* doc);

/// @brief add new flight
/// @param doc 
/// @param flight 
/// @return 0 if succeeded, error code otherwise
int document_add_flight(document_t* doc, flight_t* flight);

/// @brief remove a flight from the database
/// @param flight 
void document_remove_flight(flight_t* flight);

/// @brief free all memory allocated for doc
/// @param doc 
void destroy_document(document_t* doc);

/// @brief read the configuration file for ticket
/// @param flight 
/// @param out_info 
/// @return 0 if succeeded, error code otherwise
int document_get_flight_info(flight_t* flight, flight_info_t* out_info);

/// @brief save the configruation file for ticket
/// @param doc 
/// @param out_info 
/// @return 0 if succeeded, error code otherwise
int documeent_flight_info_save(document_t* doc, flight_info_t* out_info);

/// @brief apply the query settings in flight
/// @param info 
/// @return  0 if succeeded, error code otherwise
int document_flight_apply_query(flight_info_t* info);

/// @brief Compute how many rows are there in a flight
/// @param f 
/// @return number of rows
int document_flight_get_rows(flight_info_t* f);

/// @brief book a ticket
/// @param info 
/// @param ticket 
/// @return  0 if succeeded, error code otherwise
int  document_flight_book_ticket(flight_info_t* info, ticket_t* ticket);

/// @brief refound a ticket
/// @param info 
/// @param ticket 
/// @return  0 if succeeded, error code otherwise
int document_flight_refound_ticket(flight_info_t* info, ticket_t* ticket);


/// @brief free all memories allocated for flight_info_t structure
/// @param info 
void document_destroy_flight_info(flight_info_t* info);





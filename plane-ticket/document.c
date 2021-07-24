#define _CRT_SECURE_NO_WARNINGS

#include "document.h"
#include "error.h"
#include "helper.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* main_db_name = "main_db";

static wchar_t flight_db_name_buf[256] = { 0 };
static inline wchar_t* flight_get_db_file_name(const flight_t* f)
{
	memset(flight_db_name_buf, 0, sizeof(flight_db_name_buf));
	swprintf(flight_db_name_buf, ARRAY_SIZE(flight_db_name_buf), L"%ls-%lld-%lld-%lld-%lld-%lld-%lld",
		f->id,
		f->date.year,
		f->date.month,
		f->date.day,
		f->date.hours,
		f->date.minutes,
		f->date.seconds);

	return flight_db_name_buf;
}



static inline int flight_id_cmp(const void* a, const void* b)
{
	flight_t* fa = *(flight_t**)a;
	flight_t* fb = *(flight_t**)b;
	return strcmp(fa->id, fb->id);
}

static inline int flight_date_cmp(const void* a, const void* b)
{
	flight_t* fa = *(flight_t**)a;
	flight_t* fb = *(flight_t**)b;
	return flight_date_comp(&fa->date, &fb->date);
}

static inline int document_item_apply_query(const document_t* doc, const flight_t* f)
{
	if (!f)return FALSE;

	if (f->flags & FFLAG_DELETE)return FALSE;

	if (!doc->query.is_queried)
	{
		return TRUE;
	}

	if (doc->query.dest)
	{
		if (wcsstr(f->to, doc->query.dest) == NULL)
		{
			return FALSE;
		}
	}

	if (doc->query.date_from && doc->query.date_to)
	{
		if (flight_date_comp(&f->date, doc->query.date_from) < 0 || flight_date_comp(&f->date, doc->query.date_to) > 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

static inline int flight_item_apply_query(const flight_info_t* info, const ticket_t* t)
{
	if (!t)return FALSE;

	if (t->flags & TFLAG_DELETE)return FALSE;

	int result = TRUE;

	return result;
}

int load_document(document_t* doc)
{
	memset(doc, 0, sizeof(document_t));

	doc->header = (document_t*)malloc(sizeof(document_t));
	if (!doc->header)
	{
		return ERROR_MEMORY_ALLOC;
	}
	memset(doc->header, 0, sizeof(document_t));

	doc->flights = NULL;

	if (_access(main_db_name, 2) == -1)return ERROR_SUCCESS;

	FILE* main_db = fopen(main_db_name, "rb");
	if (!main_db)
	{
		return ERROR_OPENFILE;
	}

	if (fread(doc->header, sizeof(document_header_t), 1, main_db) != 1)
	{
		fclose(main_db);
		return ERROR_RWFILE;
	}

	doc->flights = (flight_t*)calloc(doc->header->flight_count, sizeof(flight_t));
	if (!doc->flights)
	{
		fclose(main_db);
		return ERROR_MEMORY_ALLOC;
	}
	memset(doc->flights, 0, sizeof(flight_t) * doc->header->flight_count);


	if (fread(doc->flights, sizeof(flight_t), doc->header->flight_count, main_db) != doc->header->flight_count)
	{
		fclose(main_db);
		return ERROR_RWFILE;
	}

	fclose(main_db);

	return document_apply_query(doc);
}

int save_document(document_t* doc)
{
	FILE* main_db = fopen(main_db_name, "wb");
	if (!main_db)
	{
		return ERROR_OPENFILE;
	}

	if (fwrite(doc->header, sizeof(document_header_t), 1, main_db) != 1)
	{
		fclose(main_db);
		return ERROR_RWFILE;
	}

	if (fwrite(doc->flights, sizeof(flight_t), doc->header->flight_count, main_db) != doc->header->flight_count)
	{
		fclose(main_db);
		return ERROR_RWFILE;
	}

	fclose(main_db);
	return ERROR_SUCCESS;
}

int flight_date_comp(const flight_date_t* f1, const flight_date_t* f2)
{
	if (f1->year != f2->year)
	{
		return f1->year - f2->year;
	}
	else if (f1->month != f2->month)
	{
		return f1->month - f2->month;
	}
	else if (f1->day != f2->day)
	{
		return f1->day - f2->day;
	}
	else if (f1->hours != f2->hours)
	{
		return f1->hours - f2->hours;
	}
	else if (f1->minutes != f2->minutes)
	{
		return f1->minutes - f2->minutes;
	}
	else if (f1->seconds != f2->seconds)
	{
		return f1->seconds - f2->seconds;
	}

	return 0;
}

int document_apply_query(document_t* doc)
{
	// count all valid item
	size_t total = 0;
	for (int i = 0; i < doc->header->flight_count; i++)
	{
		if (document_item_apply_query(doc, &doc->flights[i]))
		{
			total++;
		}
	}

	if (doc->result)
	{
		free(doc->result);
	}

	doc->result = calloc(total, sizeof(flight_t*));
	if (!doc->result)
	{
		return ERROR_MEMORY_ALLOC;
	}

	for (int i = 0, idx = 0; i < doc->header->flight_count && idx < total; i++)
	{
		if (document_item_apply_query(doc, &doc->flights[i]))
		{
			doc->result[idx++] = &doc->flights[i];
		}
	}

	if (doc->sort.is_sorted)
	{
		if (doc->sort.sort_flags & FLIGHT_QUERY_SORT_NAME)
		{
			qsort(doc->result, doc->header->flight_count, sizeof(flight_t*), flight_id_cmp);
		}
		else if (doc->sort.sort_flags & FLIGHT_QUERY_SORT_DATE)
		{
			qsort(doc->result, doc->header->flight_count, sizeof(flight_t*), flight_date_cmp);
		}
	}

	doc->result_count = total;

	return 0;
}

int document_add_flight(document_t* doc, flight_t* flight)
{
	doc->header->flight_count++;
	void* new_mem = realloc(doc->flights, sizeof(flight_t) * doc->header->flight_count);
	if (!new_mem)
	{
		return ERROR_MEMORY_ALLOC;
	}
	doc->flights = new_mem;

	doc->flights[doc->header->flight_count - 1] = *flight;

	return 0;
}

void document_remove_flight(flight_t* flight)
{
	flight->flags |= FFLAG_DELETE;
}

int document_get_flight_info(flight_t* flight, flight_info_t* out_info)
{
	memset(out_info, 0, sizeof(flight_info_t));
	out_info->parent = flight;

	out_info->header = (flight_header_t*)malloc(sizeof(flight_header_t));
	if (!out_info->header)
	{
		return ERROR_MEMORY_ALLOC;
	}
	memset(out_info->header, 0, sizeof(flight_header_t));

	out_info->tickets = NULL;

	wchar_t* file_name = flight_get_db_file_name(flight);

	if (_waccess(file_name, 2) == -1)return ERROR_SUCCESS;

	FILE* flight_db = _wfopen(file_name, L"rb");
	if (!flight_db)
	{
		return ERROR_OPENFILE;
	}

	if (fread(out_info->header, sizeof(flight_header_t), 1, flight_db) != 1)
	{
		fclose(flight_db);
		return ERROR_RWFILE;
	}

	out_info->tickets = (flight_t*)calloc(out_info->header->ticket_count, sizeof(ticket_t));
	if (!out_info->tickets)
	{
		fclose(flight_db);
		return ERROR_MEMORY_ALLOC;
	}
	memset(out_info->tickets, 0, sizeof(ticket_t) * out_info->header->ticket_count);


	if (fread(out_info->tickets, sizeof(ticket_t), out_info->header->ticket_count, flight_db) != out_info->header->ticket_count)
	{
		fclose(flight_db);
		return ERROR_RWFILE;
	}

	fclose(flight_db);

	return 0;
}

int documeent_flight_info_save(document_t* doc, flight_info_t* info)
{
	FILE* db = _wfopen(flight_get_db_file_name(info->parent), L"wb");
	if (!db)
	{
		return ERROR_RWFILE;
	}

	if (fwrite(info->header, sizeof(flight_header_t), 1, db) != 1)
	{
		fclose(db);
		return ERROR_RWFILE;
	}

	if (fwrite(info->tickets, sizeof(ticket_t), info->header->ticket_count, db) != info->header->ticket_count)
	{
		fclose(db);
		return ERROR_RWFILE;
	}

	fclose(db);
	return save_document(doc);
}

int document_flight_apply_query(flight_info_t* info)
{
	// count all valid item
	size_t total = 0;
	for (int i = 0; i < info->header->ticket_count; i++)
	{
		if (flight_item_apply_query(info, &info->tickets[i]))
		{
			total++;
		}
	}

	if (info->result)
	{
		free(info->result);
	}

	info->result = calloc(total, sizeof(ticket_t*));
	if (!info->result)
	{
		return ERROR_MEMORY_ALLOC;
	}

	for (int i = 0, idx = 0; i < info->header->ticket_count && idx < total; i++)
	{
		if (flight_item_apply_query(info, &info->tickets[i]))
		{
			info->result[idx++] = &info->tickets[i];
		}
	}

	info->result_count = total;

	return 0;
}

int document_flight_book_ticket(flight_info_t* info, ticket_t* ticket)
{
	info->header->ticket_count++;
	void* new_mem = realloc(info->tickets, sizeof(ticket_t) * info->header->ticket_count);
	if (!new_mem)
	{
		return ERROR_MEMORY_ALLOC;
	}

	info->tickets = new_mem;

	info->tickets[info->header->ticket_count - 1] = *ticket;

	info->parent->sold++;
	info->parent->remaining--;

	int row = 0;
	wchar_t col = 0;
	if (swscanf(ticket->seat, L"%d%c", &row, &col) != 2)
	{
		return 1;
	}
	switch (col)
	{
	case L'A':
		info->parent->ticket_bitmap[row] |= (1 << 0);
		break;
	case L'B':
		info->parent->ticket_bitmap[row] |= (1 << 1);
		break;
	case L'C':
		info->parent->ticket_bitmap[row] |= (1 << 2);
		break;
	case L'H':
		info->parent->ticket_bitmap[row] |= (1 << 3);
		break;
	case L'J':
		info->parent->ticket_bitmap[row] |= (1 << 4);
		break;
	case L'K':
		info->parent->ticket_bitmap[row] |= (1 << 5);
		break;
	default:
		return 1;
	}

	return 0;
}

int document_flight_refound_ticket(flight_info_t* info, ticket_t* ticket)
{
	ticket->flags |= TFLAG_DELETE;

	int row = 0;
	wchar_t col = 0;
	if (swscanf(ticket->seat, L"%d%c", &row, &col) != 2)
	{
		return 1;
	}
	switch (col)
	{
	case L'A':
		info->parent->ticket_bitmap[row] &= ~(1 << 0);
		break;
	case L'B':
		info->parent->ticket_bitmap[row] &= ~(1 << 1);
		break;
	case L'C':
		info->parent->ticket_bitmap[row] &= ~(1 << 2);
		break;
	case L'H':
		info->parent->ticket_bitmap[row] &= ~(1 << 3);
		break;
	case L'J':
		info->parent->ticket_bitmap[row] &= ~(1 << 4);
		break;
	case L'K':
		info->parent->ticket_bitmap[row] &= ~(1 << 5);
		break;
	default:
		return 1;
	}

	info->parent->sold--;
	info->parent->remaining++;

	return 0;
}

void document_destroy_flight_info(flight_info_t* info)
{
	free(info->header);
	free(info->tickets);
}

void destroy_document(document_t* doc)
{
	free(doc->header);
	if (doc->flights)
	{
		free(doc->flights);
	}
}

int document_flight_get_rows(flight_info_t* f)
{
	float result = (f->parent->remaining + f->parent->sold) / 6.0;
	return (int)(result + 0.5);
}
#define _CRT_SECURE_NO_WARNINGS

#include "document.h"
#include "error.h"
#include "helper.h"

#include <io.h>
#include <stdio.h>

const char* main_db_name = "main_db";

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
	if (!doc->header)
	{
		return ERROR_OPENFILE;
	}

	if (fread(doc->header, sizeof(document_header_t), 1, main_db) != 1)
	{
		fclose(main_db);
		return ERROR_RWFILE;
	}

	doc->flights = (flight_t*)malloc(sizeof(flight_t) * doc->header->flight_count);
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

	doc->result = doc->flights;

	return ERROR_SUCCESS;
}

int save_document(document_t* doc)
{
	FILE* main_db = fopen(main_db_name, "wb");
	if (!doc->header)
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

void document_remove_flight(document_t* doc, flight_t* flight)
{
	for (int i = 0; i < doc->header->flight_count; i++)
	{
		if (doc->flights[doc->header->flight_count - 1].flight_key == flight->flight_key)
		{
			doc->flights[doc->header->flight_count - 1].flags |= FFLAG_DELETE;
		}
	}
}

void destroy_document(document_t* doc)
{
	free(doc->header);
	if (doc->flights)
	{
		free(doc->flights);
	}
}

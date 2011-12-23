#include <string.h>
#include <assert.h>

#include "myutil.h"
#include "query.h"

void qset_size_query(size_query *query, unsigned int min, unsigned int max)
{
    assert(min >= 0 && max >= 0);
    query->min = min;
    query->max = max;
}

void qset_id_query(id_query *query, int id)
{
    assert(id >= 0);
    query->id = id;
}

void qset_time_query(time_query *query, unsigned int min, unsigned int max)
{
    assert(min >= 0 && max >= 0);
    query->min = min;
    query->max = max;
}

void qset_type_query(type_query *query, char *type)
{
    assert(type);
    strcpy(query->type, type);
}


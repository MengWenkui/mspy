#ifndef _SPY_H
#define _SPY_H

#include "myutil.h"
#include "partition.h"
#include "query.h"

struct spy_meta {
    partition pt;
    filter f;
};

typedef struct spy_meta spy_meta;

mylist *spy_init(char *cfile);
int spy_filter_one(spy_meta *meta, void *query, query_type type);
int spy_filter_query(spy_meta *meta, user_query *query);

#endif

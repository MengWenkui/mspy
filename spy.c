#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spy.h"

mylist *spy_init(char *cfile)
{
    mylist *plist = partition_init(cfile);
    mylist *slist = mylist_create(plist->max, myfree);
    char filterpath[P_PATH_BUF];
    for(int i = 0; i < plist->size; i++) {
        spy_meta *meta = (spy_meta *)mymalloc(sizeof(spy_meta));
        partition *pt = (partition *)plist->item[i].data;
        get_filter_path(filterpath, pt->name);
        filter_read(&meta->f, filterpath);
        meta->pt = *pt;
        mylist_append(slist, meta);
    }

    free(plist);
    return slist;
}

int spy_filter_one(spy_meta *meta, void *query, query_type type)
{
    filter *f = &meta->f;
    switch(type) {
    case SIZE_QUERY: {
        size_query *sq = (size_query *)query;
        int min_index = filter_bysize(sq->min);
        int max_index = filter_bysize(sq->max);

        int found = 0;
        for(int i = min_index; i <= max_index; i++) {
            if(filter_isset(f, i)) {
                found = 1;
                break;
            }
        }

        return found;
    }

    case UID_QUERY: {
        id_query *iq = (id_query *)query;
        int index = filter_byuid(iq->id);
        if(filter_isset(f, index)) {
            return 1;
        }
        return 0;
    }

    case GID_QUERY: {
        id_query *iq = (id_query *)query;
        int index = filter_byuid(iq->id);
        if(filter_isset(f, index)) {
            return 1;
        }
        return 0;
    }

    case ATIME_QUERY: {
          time_query *tq = (time_query *)query;

        int found = 0;
        for(int i = tq->min; i <= tq->max; i += 86400) {
            int index = filter_byatime(i);
            if(filter_isset(f, index)) {
                found = 1;
                break;
            }
        }

        return found;
    }

    case CTIME_QUERY: {
        time_query *tq = (time_query *)query;

        int found = 0;
        for(int i = tq->min; i <= tq->max; i += 86400) {
            int index = filter_byctime(i);
            if(filter_isset(f, index)) {
                found = 1;
                break;
            }
        }

        return found;
    }

    case MTIME_QUERY: {
         time_query *tq = (time_query *)query;

        int found = 0;
        for(int i = tq->min; i <= tq->max; i += 86400) {
            int index = filter_bymtime(i);
            if(filter_isset(f, index)) {
                found = 1;
                break;
            }
        }

        return found;
    }

    case TYPE_QUERY: {
        type_query *tq = (type_query *)query;

        int index = filter_bytype(tq->type);
        if(filter_isset(f, index)) {
            return 1;
        }
        return 0;
    }

    }

    return 0;
}

int spy_filter_query(spy_meta *meta, user_query *query)
{
    int found = 1;
    if(query->uid != -1 ) {
        id_query iq;
        qset_id_query(&iq, query->uid);
        found = spy_filter_one(meta, &iq, UID_QUERY);
        if(!found) {
            return found;
        }
    }

    if(query->gid != -1) {
        id_query iq;
        qset_id_query(&iq, query->gid);
        found = spy_filter_one(meta, &iq, GID_QUERY);
        if(!found) {
            return found;
        }
    }

    if(query->size_min != -1 && query->size_max != -1) {
        size_query sq;
        qset_size_query(&sq, query->size_min, query->size_max);
        found = spy_filter_one(meta, &sq, SIZE_QUERY);
        if(!found) {
            return found;
        }
    }

    if(query->atime_min != -1 && query->atime_max != -1) {
        time_query tq;
        qset_time_query(&tq, query->atime_min, query->atime_max);
        found = spy_filter_one(meta, &tq, ATIME_QUERY);
        if(!found) {
            return found;
        }
    }

    if(query->ctime_min != -1 && query->ctime_max != -1) {
        time_query tq;
        qset_time_query(&tq, query->ctime_min, query->ctime_max);
        found = spy_filter_one(meta, &tq, CTIME_QUERY);
        if(!found) {
            return found;
        }
    }

    if(query->mtime_min != -1 && query->mtime_max != -1) {
        time_query tq;
        qset_time_query(&tq, query->mtime_min, query->mtime_max);
        found = spy_filter_one(meta, &tq, MTIME_QUERY);
        if(!found) {
            return found;
        }
    }

    if(strlen(query->type)) {
        type_query tq;
        qset_type_query(&tq, query->type);
        found = spy_filter_one(meta, &tq, TYPE_QUERY);
        if(!found) {
            return found;
        }
    }

    return found;
}


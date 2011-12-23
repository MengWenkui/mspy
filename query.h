#ifndef _QUERY_H
#define _QUERY_H

struct user_query {
    unsigned int size_min;
    unsigned int size_max;
    int uid;
    int gid;
    unsigned int atime_min;
    unsigned int atime_max;
    unsigned int ctime_min;
    unsigned int ctime_max;
    unsigned int mtime_min;
    unsigned int mtime_max;
    char type[8];
};

typedef struct user_query user_query;

enum query_type {
    SIZE_QUERY,
    UID_QUERY,
    GID_QUERY,
    ATIME_QUERY,
    CTIME_QUERY,
    MTIME_QUERY,
    TYPE_QUERY
};

typedef enum query_type query_type;

struct size_query {
    unsigned int min;
    unsigned int max;
};

typedef struct size_query size_query;

struct time_query {
    unsigned int min;
    unsigned int max;
};

typedef struct time_query time_query;

struct id_query {
    unsigned int id;
};

typedef struct id_query id_query;

struct type_query {
    char type[8];
};

typedef struct type_query type_query;

void qset_id_query(id_query *query, int id);
void qset_size_query(size_query *query, unsigned int min, unsigned int max);
void qset_time_query(time_query *query, unsigned int min, unsigned int max);
void qset_type_query(type_query *query, char *type);

#endif

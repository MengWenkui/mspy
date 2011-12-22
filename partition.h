#ifndef _PARTITION_H
#define _PARTITION_H

#define P_DATA_DIR "data"
#define P_FILTER_SUFFIX ".filter"
#define P_META_SUFFIX ".meta"
#define P_CFG_PATH "partition.conf"
#define P_LIST_BUF 16
#define P_LINE_BUF 1024
#define P_NAME_BUF 128
#define P_PATH_BUF 1024

#include <sys/stat.h>
#include "filter.h"

struct partition {
    char name[P_NAME_BUF];
};

typedef struct partition partition;

struct file_meta {
    uint16_t psiz;
    uint16_t uid;
    uint16_t gid;
    uint16_t mode;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t ino;
    char path[0];
};

typedef struct file_meta file_meta;


/*
 * get partition list from config file
 */
mylist *partition_init(const char *cfile);

/*
 * get the meta filename and
 * filter filename
 */
void get_meta_path(char *metapath, const char *path);
void get_filter_path(char *filterpath, const char *path);

/*
 * metadata operation API
 */
file_meta *meta_new(struct stat *st, const char *fullpath);
void meta_append(file_meta *meta, FILE *fp);
void meta_filter(file_meta *meta, filter *f);
int meta_write(mylist *mlist, const char *path);
mylist *meta_read(const char *path);


#endif

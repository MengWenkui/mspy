#ifndef _FILTER_H
#define _FILTER_H

#include <stdint.h>

#define F_UID_OFFSET   0          /* start of uid filter */
#define F_GID_OFFSET   1024       /* start of gid filter */
#define F_SIZE_OFFSET  2048       /* start of size filter */
#define F_ATIME_OFFSET 4096       /* start of atime filter */
#define F_CTIME_OFFSET 5120       /* start of ctime filter */
#define F_MTIME_OFFSET 6144       /* start of mtime filter */
#define F_TYPE_OFFSET  7168       /* start of mode filter */
#define F_TOTAL_SIZE   8192      /* total size of the filter */

struct filter {
    uint8_t data[F_TOTAL_SIZE/8];
};

typedef struct filter filter;

void filter_set(filter *f, int index);
void filter_clear(filter *f, int index);
int  filter_isset(filter *f, int index);
void filter_read(filter *f, const char *path);
void filter_write(filter *f, const char *path);

int filter_bysize(unsigned int size);
int filter_byuid(unsigned int uid);
int filter_bygid(unsigned int gid);
int filter_byatime(unsigned int atime);
int filter_byctime(unsigned int ctime);
int filter_bymtime(unsigned int mtime);
int filter_bytype(char *type);

void filter_set_size(filter *f, unsigned int size);
void filter_set_uid(filter *f, unsigned int uid);
void filter_set_gid(filter *f, unsigned int gid);
void filter_set_atime(filter *f, unsigned int atime);
void filter_set_ctime(filter *f, unsigned int ctime);
void filter_set_mtime(filter *f, unsigned int mtime);
void filter_set_type(filter *f, char *type);


#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "myutil.h"
#include "filter.h"

const char suffix[][8] = {"asp", "avi",
    "bak", "bat", "bin", "bmp", "cab",
    "cfg", "cgi", "chm", "cmd", "com",
    "c", "c++", "cpp", "cc", "dat", "dbf",
    "dll", "doc","drv","eml", "exe", "gif",
    "hlp", "htm","html", "ico", "img", "inf",
    "ini","iso", "jar", "java", "jpg", "jpeg",
    "log", "lrc", "mid", "mov", "mp3", "mpg",
    "mpeg", "msg", "msi", "pdf", "php", "ppt",
    "rar", "reg", "rm","rmvb", "swf", "sys",
    "tif", "ttf", "txt", "wav", "wav", "xls",
    "xlt", "zip", "h", "hpp"
};


void filter_set(filter *f, int index)
{
    assert(f && index >= 0);
    int idx = index / 8;
    int bit = index % 8;
    int mask = 0x80 >> bit;
    f->data[idx] |= mask;
}

void filter_clear(filter *f, int index)
{
    assert(f && index >= 0);
    int idx = index / 8;
    int bit = index % 8;
    int mask = ~(0x80 >> bit);
    f->data[idx] &= mask;
}

int filter_isset(filter *f, int index)
{
    assert(f && index >= 0);
    int idx = index / 8;
    int bit = index % 8;
    int mask = 0x80 >> bit;
    return f->data[idx] & mask;
}


void filter_read(filter *f, const char *path)
{
    FILE *fp = fopen(path, "r");
    if(!fp) {
        myfatal("fopen");
    }

    fread(f->data, sizeof(char), sizeof(f->data), fp);
    fclose(fp);
}

void filter_write(filter *f, const char *path)
{
    FILE *fp = fopen(path, "w+");
    if(!fp) {
        myfatal("fopen");
    }

    fwrite(f->data, sizeof(char), sizeof(f->data), fp);
    fclose(fp);
}

int index_bysize(unsigned int size)
{
    if(size < 1024) {  // < 1KB
        return 0;
    } else if(size < 1024 * 1024) { // [n, n+1)KB
        return size / 1024;
    } else if(size < 1024 * 1024 * 1024) {
        return 1024 + size / (1024 * 1024);  // [n, n+1)MB
    } else {
        return 2048 - 1;
    }
}

int index_bytime(unsigned int time)
{
    int day = time - time % 86400;
    return day % 1021; // primer
}

int index_byid(unsigned int id)
{
    return id % 1024;
}

int index_bytype(char *type)
{
    if(!type) {
        return 0;
    }

    char *suf = type + strlen(type) - 1;
    while(*suf != '.' && suf >= type) suf--;
    suf++;

    int len = sizeof(suffix) / sizeof(suffix[0]);
    for(int i = 0; i < len; i++) {
        if(!strcasecmp(suf, suffix[i])) {
            return i + 1;
        }
    }

    return 0;
}

int filter_bysize(unsigned int size)
{
    return index_bysize(size) + F_SIZE_OFFSET;
}

int filter_byuid(unsigned int uid)
{
    return index_byid(uid) + F_UID_OFFSET;
}

int filter_bygid(unsigned int gid)
{
    return index_byid(gid) + F_GID_OFFSET;
}

int filter_byatime(unsigned int atime)
{
    return index_bytime(atime) + F_ATIME_OFFSET;
}

int filter_byctime(unsigned int ctime)
{
    return index_bytime(ctime) + F_CTIME_OFFSET;
}

int filter_bymtime(unsigned int mtime)
{
    return index_bytime(mtime) + F_MTIME_OFFSET;
}

int filter_bytype(char *type)
{
    return index_bytype(type) + F_TYPE_OFFSET;
}

void filter_set_size(filter *f, unsigned int size)
{
    int index = filter_bysize(size);
    filter_set(f, index);
}

void filter_set_uid(filter *f, unsigned int uid)
{
    int index = filter_byuid(uid);
    filter_set(f, index);
}

void filter_set_gid(filter *f, unsigned int gid)
{
    int index = filter_bygid(gid);
    filter_set(f, index);
}

void filter_set_atime(filter *f, unsigned int atime)
{
    int index = filter_byatime(atime);
    filter_set(f, index);
}

void filter_set_ctime(filter *f, unsigned int ctime)
{
    int index = filter_byctime(ctime);
    filter_set(f, index);
}

void filter_set_mtime(filter *f, unsigned int mtime)
{
    int index = filter_bymtime(mtime);
    filter_set(f, index);
}

void filter_set_type(filter *f, char *type)
{
    int index = filter_bytype(type);
    filter_set(f, index);
}

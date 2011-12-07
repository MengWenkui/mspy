#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "myutil.h"
#include "partition.h"
#include "filter.h"

void trim_space(char *str)
{
    char *s = str;
    char *e = str + strlen(str) - 1;
    while(*s == ' ' && *s != '\0') s++;
    if(*s == '\0') {
        str[0] = '\0';
        return;
    }
    while(*e == ' ' || *e == '\n' || *e == '\r') e--;
    strncpy(str, s, e - s + 1);
    str[e-s+1] = '\0';
}

mylist *partition_init(const char *cfile)
{
    FILE *fp = fopen(cfile, "r");
    if(!fp) {
       myfatal("fopen");
    }

    mylist *list = mylist_create(P_LIST_BUF, myfree);

    char line[P_LINE_BUF];
    partition *pt;
    while(fgets(line, P_LINE_BUF, fp) != NULL) {
        if(line[0] == '#') {  // comment line
            continue;
        }
        trim_space(line);
        if(line[0] == '\0') {  // blank line
            continue;
        }

        partition *pt = (partition *)mymalloc(sizeof(partition));
        strcpy(pt->name, line);
        mylist_append(list, pt);
    }

    fclose(fp);
    return list;
}

void encode_name(char *encoded_name, const char *name)
{
    int len = strlen(name);
    for(int i = 0; i < len; i++) {
        if(name[i] == '/') {
            encoded_name[i] = '%';
        } else {
            encoded_name[i] = name[i];
        }
    }
    encoded_name[len] = '\0';
}

void get_meta_path(char *metapath, const char *path)
{
    char tpath[1024];
    encode_name(tpath, path);
    sprintf(metapath, "%s/%s", P_DATA_DIR, tpath);
    strcat(metapath, P_META_SUFFIX);
}

void get_filter_path(char *filterpath, const char *path)
{
    char tpath[1024];
    encode_name(tpath, path);
    sprintf(filterpath, "%s/%s", P_DATA_DIR, tpath);
    strcat(filterpath, P_FILTER_SUFFIX);
}


file_meta *meta_new(struct stat *st, const char *fullpath)
{
    file_meta *meta = (file_meta *)mymalloc(sizeof(file_meta) + P_PATH_BUF);
    meta->psiz = strlen(fullpath) + 1; // 1 for '\0'
    meta->uid = st->st_uid;
    meta->gid = st->st_gid;
    meta->mode = st->st_mode;
    meta->size = st->st_size;
    meta->atime = st->st_atime;
    meta->ctime = st->st_ctime;
    meta->mtime = st->st_mtime;
    strcpy(meta->path, fullpath);

    return meta;
}

void meta_append(file_meta *meta, FILE *fp)
{
    int wsiz = meta->psiz + sizeof(file_meta);
    fwrite(meta, wsiz, 1, fp);
}



void meta_filter(file_meta *meta, filter *f)
{
    filter_set_size(f, meta->size);
    filter_set_uid(f, meta->uid);
    filter_set_gid(f, meta->gid);
    filter_set_atime(f, meta->atime);
    filter_set_ctime(f, meta->ctime);
    filter_set_mtime(f, meta->mtime);
    filter_set_type(f, meta->path);
}


int meta_write(mylist *mlist, const char *path)
{
    FILE *fp = fopen(path, "w+");
    if(!fp) {
        myfatal("fopen");
    }

    for(int i = 0; i < mlist->size; i++) {
        file_meta *meta = (file_meta *)mlist->item[i].data;
        int wsiz = sizeof(file_meta) + meta->psiz;
        fwrite(meta, wsiz, 1, fp);
    }

    fclose(fp);
    return 0;
}

mylist *meta_read(const char *path)
{
    mylist *mlist = mylist_create(1024, myfree);
    FILE *fp = fopen(path, "r");
    if(!fp) {
        myfatal("fopen");
    }

    while(1) {
        file_meta *meta = (file_meta *)mymalloc(sizeof(file_meta) + P_PATH_BUF);
        if(fread(meta, sizeof(file_meta), 1, fp) != 1)
            break;
        fread(meta->path, meta->psiz, 1, fp);
        mylist_append(mlist, meta);
    }
    fclose(fp);
    return mlist;
}


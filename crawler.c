#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "myutil.h"
#include "partition.h"

#define FILENO_MAX 1024 * 1024

void get_full_path(char *fullpath, const char *dirname, const char *filename)
{
    int dirlen = strlen(dirname);
    if(dirname[dirlen-1] == '/') {
        sprintf(fullpath, "%s%s", dirname, filename);
    } else {
        sprintf(fullpath, "%s/%s", dirname, filename);
    }
}

int do_crawl_meta(const char *dirpath, FILE *fp, filter *f)
{
    assert(dirpath && fp && f);
    DIR *dirp = opendir(dirpath);
    if(!dirp) {
        perror("opendir");
        return -1;
    }

    struct stat st;
    struct dirent *dir;
    char fullpath[P_PATH_BUF];
    while((dir = readdir(dirp)) != NULL) {
        if(!strcmp(dir->d_name, ".") ||
           !strcmp(dir->d_name, "..")) {
            continue;
        }

        get_full_path(fullpath, dirpath, dir->d_name);
        if(lstat(fullpath, &st) < 0) {
            continue;
        }

        file_meta *meta = meta_new(&st, fullpath);
        meta_append(meta, fp);
        meta_filter(meta, f);

        if(S_ISDIR(st.st_mode)) {
            do_crawl_meta(fullpath, fp, f);
        }
    }

    closedir(dirp);

    return 0;
}

int crawl_meta(partition *pt)
{
    assert(pt);
    char filterpath[P_PATH_BUF];
    char metapath[P_PATH_BUF];

    get_filter_path(filterpath, pt->name);
    get_meta_path(metapath, pt->name);

    struct rlimit rlim;
    rlim.rlim_cur = FILENO_MAX;
    rlim.rlim_max = FILENO_MAX;
    setrlimit(RLIMIT_NOFILE, &rlim);

    FILE *fp = fopen(metapath, "w+");
    if(!fp) {
        myfatal("fopen");
    }

    filter *f = (filter *)mymalloc(sizeof(filter));
    memset(f->data, 0, sizeof(f->data));
    do_crawl_meta(pt->name, fp, f);
    filter_write(f, filterpath);

    myfree(f);
    fclose(fp);
    return 0;
}


int crawl_run(const char *cfile)
{
    assert(cfile);
    mylist *plist = partition_init(cfile);
    for(int i = 0; i < plist->size; i++) {
        partition *pt = (partition *)plist->item[i].data;
        crawl_meta(pt);
    }

    mylist_destroy(plist);
    return 0;
}


int main(int argc, char *argv[])
{
    if(3 != argc) {
        printf("./mspy-crawler config-file datadir\n");
        return -1;
    }
    if(0 != access(argv[2], F_OK)) {
        mkdir(argv[2], 0755);
    }
    crawl_run(argv[1]);
    return 0;
}


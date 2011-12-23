#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>

#include "spy.h"

void usage()
{
    printf("mspy-search: a metadata search engine\n");
    printf("[query string format]:\n");
    printf("query [-s min max] [-u uid] [-g gid] \n");
    printf("      [-a start end] [-c start end]\n");
    printf("      [-m start end] [-t suffix]\n");
}

int parse_size(const char *arg, int pos)
{
    if(!strcmp(arg, "*")) {
        if(pos == 1) {
            return 0;
        } else if(pos == 2) {
            return 1024 * 1024 * 1024;
        }
    } else {
        int len = strlen(arg);
        char buf[10];
        strcpy(buf, arg);

        if(arg[len-1] == 'b' || arg[len-1] == 'B') {
            buf[len-1] = '\0';
            return atoi(buf);
        } else if(arg[len-1] == 'k' || arg[len-1] == 'K') {
            buf[len-1] = '\0';
            return atoi(buf) * 1024;
        } else if(arg[len-1] == 'm' || arg[len-1] == 'M'){
            buf[len-1] = '\0';
            return atoi(buf) * 1024 * 1024;
        } else if(arg[len-1] == 'g' || arg[len-1] == 'G') {
            buf[len-1] = '\0';
            return atoi(buf) * 1024 * 1024 * 1024;
        } else {
            return atoi(buf);
        }
    }

    return -1;
}

int parse_uid(const char *arg)
{
    if(isdigit(arg[0])) {
        return atoi(arg);
    }

    char line[1024];
    char id[128];
    FILE *fp = fopen("/etc/passwd", "r");
    while(fgets(line, 1023, fp) != NULL) {
        if(strstr(line, arg)) {
            sscanf(line, "%*[^:]:%*[^:]:%[^:]", id);
            fclose(fp);
            return atoi(id);
        }
    }
    fclose(fp);
    return -1;
}

int parse_gid(const char *arg)
{
    if(isdigit(arg[0])) {
        return atoi(arg);
    }

    char line[1024];
    char id[128];
    FILE *fp = fopen("/etc/passwd", "r");
    while(fgets(line, 1023, fp) != NULL) {
        if(strstr(line, arg)) {
            sscanf(line, "%*[^:]:%*[^:]:%*[^:]%[^:]", id);
            fclose(fp);
            return atoi(id);
        }
    }
    fclose(fp);
    return -1;
}

int parse_time(const char *arg, int pos)
{
    int year;
    int mon;
    int mday;
    struct tm qtime;

    if(!strcmp(arg, "*")) {
        if(pos == 1) {
            year = 1970;
            mon = 1;
            mday = 1;
        } else if(pos ==2) {
            time_t now = time(NULL);
            struct tm *tnow = gmtime(&now);
            year = tnow->tm_year;
            mon = tnow->tm_mon;
            mday = tnow->tm_mday;
        }
    } else {
        sscanf(arg, "%d-%d-%d", &year, &mon, &mday);
    }

    qtime.tm_year = year -1900;
    qtime.tm_mon = mon - 1;
    qtime.tm_mday = mday;
    qtime.tm_hour = 0;
    qtime.tm_min = 0;
    qtime.tm_sec = 0;
    qtime.tm_isdst = 0;

    return mktime(&qtime);
}

int parse_query(user_query *query, char *qstr)
{
    int argc = 0;
    char *argv[30];
    char *p;
    char *delim = " ";

    for(p = qstr; ; p = NULL) {
        char *token = strtok(p, delim);
        if(token == NULL) {
            break;
        }
        argv[argc++] = token;
    }

    if(argc < 3) {
        return -1;
    }

    if(strcmp(argv[0], "query")) {
        return -1;
    }

    query->uid = -1;
    query->gid = -1;
    query->size_min = -1;
    query->size_max = -1;
    query->atime_min = -1;
    query->atime_max = -1;
    query->ctime_min = -1;
    query->ctime_max = -1;
    query->mtime_min = -1;
    query->mtime_max = -1;
    query->type[0] = '\0';

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "--size") || !strcmp(argv[i], "-s")) {
            if(i + 2 >= argc) {
                return -1;
            }
            query->size_min = parse_size(argv[i+1], 1);
            query->size_max = parse_size(argv[i+2], 2);
            i += 2;
        } else if(!strcmp(argv[i], "--uid") || !strcmp(argv[i], "-u")) {
            if(i + 1 >= argc) {
                return -1;
            }
            query->uid = parse_uid(argv[i+1]);
            i += 1;
        } else if(!strcmp(argv[i], "--gid") || !strcmp(argv[i], "-g")) {
            if(i + 1 >= argc) {
                return -1;
            }
            query->gid = parse_gid(argv[i+1]);
            i += 1;
        } else if(!strcmp(argv[i], "--atime") || !strcmp(argv[i], "-a")) {
            if(i + 2 >= argc) {
                return -1;
            }
            query->atime_min = parse_time(argv[i+1], 1);
            query->atime_max = parse_time(argv[i+2], 2);
            i += 2;
        } else if(!strcmp(argv[i], "--ctime") || !strcmp(argv[i], "-c")) {
            if(i + 2 >= argc) {
                return -1;
            }
            query->ctime_min = parse_time(argv[i+1], 1);
            query->ctime_max = parse_time(argv[i+2], 2);
            i += 2;
        } else if(!strcmp(argv[i], "--mtime") || !strcmp(argv[i], "-m")) {
            if(i + 2 >= argc) {
                return -1;
            }
            query->ctime_min = parse_time(argv[i+1], 1);
            query->ctime_max = parse_time(argv[i+2], 2);
            i += 2;
        } else if(!strcmp(argv[i], "--type") || !strcmp(argv[i], "-t")) {
            if(i + 1 >= argc) {
                return -1;
            }
            strcpy(query->type, argv[i+1]);
            query->type[7] = '\0';
            i += 1;
        } else {
            return -1;
        }
    }

    return 0;
}

int is_matched(file_meta *meta, user_query *query)
{
    int matched = 1;

    /* match uid */
    if(query->uid != -1) {
        matched = (meta->uid == query->uid);
        if(!matched) {
            return matched;
        }
    }

    /* match gid */
    if(query->gid != -1) {
        matched = (meta->gid == query->gid);
        if(!matched) {
            return matched;
        }
    }

    /* match file size */
    if(query->size_min != -1 && query->size_max != -1) {
        matched = ((meta->size >= query->size_min) &&
                   (meta->size <= query->size_max));
    }else if(query->size_min != -1) {
        matched = (meta->size >= query->size_min);
    } else if(query->size_max != -1) {
        matched = (meta->size <= query->size_max);
    }

    if(!matched) {
        return matched;
    }


    /* match atime */
    if(query->atime_min != -1 && query->atime_max != -1) {
        matched = ((meta->atime >= query->atime_min) &&
                   (meta->atime <= query->atime_max));
    }else if(query->atime_min != -1) {
        matched = (meta->atime >= query->atime_min);
    } else if(query->atime_max != -1) {
        matched = (meta->atime <= query->atime_max);
    }

    if(!matched) {
        return matched;
    }

    /* match ctime */
    if(query->ctime_min != -1 && query->ctime_max != -1) {
        matched = ((meta->ctime >= query->ctime_min) &&
                   (meta->ctime <= query->ctime_max));
    }else if(query->ctime_min != -1) {
        matched = (meta->ctime >= query->ctime_min);
    } else if(query->ctime_max != -1) {
        matched = (meta->ctime <= query->ctime_max);
    }

    if(!matched) {
        return matched;
    }

    /* match mtime */
    if(query->mtime_min != -1 && query->mtime_max != -1) {
        matched = ((meta->mtime >= query->mtime_min) &&
                   (meta->mtime <= query->mtime_max));
    }else if(query->mtime_min != -1) {
        matched = (meta->mtime >= query->mtime_min);
    } else if(query->ctime_max != -1) {
        matched = (meta->mtime <= query->mtime_max);
    }

    if(!matched) {
        return matched;
    }

    if(strlen(query->type)) {
        char *suf = meta->path + strlen(meta->path)
                - strlen(query->type);
        matched = !(strcasecmp(suf, query->type));
    }

    return matched;

}

int do_search(partition *pt, user_query *query)
{
    char metapath[P_PATH_BUF];
    get_meta_path(metapath, pt->name);

    FILE *fp = fopen(metapath, "r");
    if(!fp) {
        perror("fopen");
        return 0;
    }

    file_meta *meta = (file_meta*)mymalloc(sizeof(file_meta) + P_PATH_BUF);

    int count = 0;
    while(1) {
        if(fread(meta, sizeof(file_meta), 1, fp) != 1) {
            break;
        }
        fread(meta->path, meta->psiz, 1, fp);

        if(is_matched(meta, query)) {
            count++;
        }
    }

    fclose(fp);
    return count;
}

int main(int argc, char *argv[])
{
    mylist *slist = spy_init("bloom/partition.conf");
    user_query *query = (user_query *)mymalloc(sizeof(user_query));

    usage();

    while(1) {
        char qstr[100];
        printf("\n\nEnter query >> ");
        fgets(qstr, 100, stdin);
        qstr[strlen(qstr) - 1] = '\0';

        if(!strcmp(qstr, "quit")) {
            break;
        }

        if(parse_query(query, qstr) < 0) {
            usage();
            continue;
        }

        long start, end;
        start = clock();
        for(int i = 0; i < slist->size; i++) {
            spy_meta *meta = (spy_meta *)slist->item[i].data;
            if(spy_filter_query(meta, query)) {
                int count = do_search(&meta->pt, query);
                printf("[%s]: %d results matched\n", meta->pt.name, count);
            }
        }
        end = clock();

        printf("\nquery time: %lf seconds\n", ((double)(end-start)) / CLOCKS_PER_SEC);
    }

    myfree(query);
    myfree(slist);

    return 0;
}



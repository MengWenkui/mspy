#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>


std::map<int, int> size_map;
std::map<int, int> uid_map;
std::map<int, int> gid_map;
std::map<int, int> atime_map;
std::map<int, int> ctime_map;
std::map<int, int> mtime_map;

int index_bysize(unsigned int size)
{
    if(size < 1024) {
        return 0;
    } else if(size < 1024 * 1024) {
        return size / 1024;
    } else if(size < 1024 * 1024 * 1024) {
        return 1024 + size / (1024 * 1024);
    } else {
        return 2048 - 1;
    }
}

int index_bytime(unsigned int time)
{
    int day = time - time % 86400;
    return day;
}

int index_byid(unsigned int id)
{
    return id;
}


void parse_stat(struct stat *st)
{
    int size = index_bysize(st->st_size);
    int uid = index_byid(st->st_uid);
    int gid = index_byid(st->st_gid);
    int atime = index_bytime(st->st_atime);
    int ctime = index_bytime(st->st_ctime);
    int mtime = index_bytime(st->st_mtime);

    size_map[size]++;
    uid_map[uid]++;
    gid_map[gid]++;
    atime_map[atime]++;
    ctime_map[ctime]++;
    mtime_map[mtime]++;
}

int dir_traverse(const char *dir_name)
{
    DIR *dirp = opendir(dir_name);
    if(!dirp) {
        perror("opendir");
        return -1;
    }

    struct stat st;
    struct dirent *dir;
    char fullpath[1024];
    while((dir = readdir(dirp)) != NULL) {
        if(!strcmp(dir->d_name, ".") || // 考虑当前目录和上级目录，否则会死循环
           !strcmp(dir->d_name, "..")) {
            continue;
        }


        parse_stat(&st);
        sprintf(fullpath, "%s/%s", dir_name, dir->d_name); //获取全局路径
        if(lstat(fullpath, &st) < 0) {
            perror("lstat");
            continue;
        }

        if(S_ISDIR(st.st_mode)) {
            dir_traverse(fullpath); // 递归遍历子目录
        }
    }

    closedir(dirp);
    return 0;
}



int main(int argc, char *argv[])
{
    if(argc != 2 || argv[1][0] != '/') {
        printf("full path expected\n");
        return -1;
    }

    dir_traverse(argv[1]);

    std::map<int, int>::iterator iter;

    printf("#### atime information\n");
    for(iter = atime_map.begin(); iter != atime_map.end(); iter++) {
        int first = iter->first;
        int second = iter->second;
        struct tm *t = gmtime((time_t *)&first);
        printf("%04d-%02d-%02d\t%d\n",
               t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, second);
    }

    printf("\n\n###############\n\n");


    printf("#### ctime information\n");
    for(iter = ctime_map.begin(); iter != ctime_map.end(); iter++) {
        int first = iter->first;
        int second = iter->second;
        struct tm *t = gmtime((time_t *)&first);
        printf("%04d-%02d-%02d\t%d\n",
               t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, second);
    }

    printf("\n\n###############\n\n");

    printf("#### mtime information\n");
    for(iter = mtime_map.begin(); iter != mtime_map.end(); iter++) {
        int first = iter->first;
        int second = iter->second;
        struct tm *t = gmtime((time_t *)&first);
        printf("%04d-%02d-%02d\t%d\n",
               t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, second);
    }


    printf("\n\n###############\n\n");

    printf("#### uid information\n");
    for(iter = uid_map.begin(); iter != uid_map.end(); iter++) {
        int first = iter->first;
        int second = iter->second;
        printf("%d\t%d\n", first, second);
    }

    printf("\n\n###############\n\n");

    printf("#### gid information\n");
    for(iter = gid_map.begin(); iter != gid_map.end(); iter++) {
        int first = iter->first;
        int second = iter->second;
        printf("%d\t%d\n", first, second);
    }

    printf("\n\n###############\n\n");

    printf("#### size information\n");
    for(iter = size_map.begin(); iter != size_map.end(); iter++) {
        int first = iter->first;
        int second = iter->second;

        if(first < 1024) {
            printf("%dK\t%d\n", first, second);
        } else if(first < 2048) {
            printf("%dM\t%d\n", first - 1024, second);
        }
    }
    return 0;
}


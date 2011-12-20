#include <stdio.h>
#include <sys/time.h>
#include <vector>
#include <string.h>
#include "skdtree.h"
#include "../filter.h"
#include "../myutil.h"
#include "../partition.h"

double get_interval(struct timeval start, struct timeval end)
{

    uint64_t val = (end.tv_sec - start.tv_sec) * 1000000 +
        (end.tv_usec - start.tv_usec);
    return (double)val / 1000;
}

inline bool encloses(file_meta *m, const meta_info_t &vmin, const meta_info_t &vmax)
{
    if(m->ino >= vmin.d[0] && m->ino <= vmax.d[0] &&
       m->size >= vmin.d[1] && m->size <= vmax.d[1] &&
       m->uid >= vmin.d[2] && m->uid <= vmax.d[2] && 
       m->atime >= vmin.d[3] && m->atime <= vmax.d[3] &&
       m->ctime >= vmin.d[4] && m->ctime <= vmax.d[4] &&
       m->mtime >= vmin.d[5] && m->mtime <= vmax.d[5]) {
        return true;
    }
    return false;
}

void mylist_find_range(mylist *mlist, const meta_info_t &vmin, const meta_info_t &vmax, std::vector<file_meta *> &result)
{
    for(int i = 0; i < mlist->size; i++) {
        file_meta *m = (file_meta *)mlist->item[i].data;
        if(true == encloses(m, vmin, vmax)) {
            result.push_back(m);
        }
   }
}

void write_xls(const char * filename, double result[][100], int rows, int cols)
{
    FILE *fp = fopen(filename, "w+");
    if(NULL == fp) {
        perror("fopen");
        return;
    }

    char buf[10240];
    char ele[100];
    for(int i = 0; i < rows; i++) {
        buf[0] = '\0';
        for(int j = 0; j < cols; j++) {
            sprintf(ele, "%lf\t", result[i][j]);
            strcat(buf, ele);
            printf("%lf\t", result[i][j]);
        }
        strcat(buf, "\n");
        fwrite(buf, 1, strlen(buf), fp);
        printf("\n");
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    if(3 != argc) {
        printf("./meta_kdtree metapath resultpath\n");
        return -1;
    }

    skdtree_t se(6);
    mylist *mlist = meta_read(argv[1]);

    int base = 10000;
    int scale = 150000;
    double result[10][100];
    struct timeval start, end;
    int idx = 0;

    // insert every file's metadata to simple kdtree
    //
    for( ;  base <= scale; base += 10000) {
        
        printf("testing set with %d files ...\n", base);
        
        for(int i = 0; i < base; i++) {
            file_meta *m = (file_meta *)mlist->item[i].data;
            meta_info_t *mi = new meta_info_t(m->ino, m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
            se.insert(mi);
        }

        meta_info_t min(0, 0, 0, 1024 * 1024, 0, 0, 1209492430);
        meta_info_t max(0xffffffff, 0, 0, 2 * 1024 * 1024, 0xffffffff, 0xffffffff, 0xffffffff);
        region_t reg(min, max);
        std::vector<meta_info_t *> v;
        std::vector<file_meta *> r;


        // list
        gettimeofday(&start, NULL);
        mylist_find_range(mlist, min, max, r);
        gettimeofday(&end, NULL);
        result[0][idx] = get_interval(start, end);

        // kdtree
        gettimeofday(&start, NULL);
        se.find_within_range(reg, v);
        gettimeofday(&end, NULL);
        result[1][idx] = get_interval(start, end);
        
        // balanced kdtree
        se.balance();
        v.clear();
        gettimeofday(&start, NULL);
        se.find_within_range(reg, v);
        gettimeofday(&end, NULL);
        result[2][idx] = get_interval(start, end);

        idx++;
        se.clear();
    }


    write_xls(argv[2], result, 3, idx);
   
    return 0;
    
}

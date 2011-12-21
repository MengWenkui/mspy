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
 
        // kdtree
        gettimeofday(&start, NULL);
        se.balance();
        gettimeofday(&end, NULL);
        result[0][idx] = get_interval(start, end);
        
        // balanced kdtree
        gettimeofday(&start, NULL);
        se.dump_tofile("kdtree.img");
        gettimeofday(&end, NULL);
        result[1][idx] = get_interval(start, end);

        idx++;
        se.clear();
    }


    write_xls(argv[2], result, 2, idx);
   
    return 0;
    
}

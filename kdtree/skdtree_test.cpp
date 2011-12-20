#include <stdio.h>
#include <sys/time.h>
#include <vector>
#include "skdtree.h"
#include "../filter.h"
#include "../myutil.h"
#include "../partition.h"

void print_interval(struct timeval start, struct timeval end)
{

    uint64_t val = (end.tv_sec - start.tv_sec) * 1000000 +
        (end.tv_usec - start.tv_usec);
    printf("%lluus\n",val);
}


void dump_test(skdtree_t *kdtree)
{
    std::vector<meta_info_t *> result;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    kdtree->dump(result);
    gettimeofday(&end, NULL);
    printf("height: %d\n", kdtree->height());
    print_interval(start, end);
    printf("%d\n", result.size());

    printf("-------------\n");

    kdtree->balance();
    result.clear();
    gettimeofday(&start, NULL);
    kdtree->dump(result);
    gettimeofday(&end, NULL);
    printf("height: %d\n", kdtree->height());
    print_interval(start, end);
    printf("%d\n", result.size()); 
}

int main(int argc, char *argv[])
{
    if(2 != argc) {
        printf("./meta_kdtree metapath\n");
        return -1;
    }

    skdtree_t se(6);
    mylist *mlist = meta_read(argv[1]);
    printf("partiton size = %d\n", mlist->size);

    // insert every file's metadata to simple kdtree
    for(int i = 0; i < mlist->size; i++) {
        file_meta *m = (file_meta *)mlist->item[i].data;
        meta_info_t *mi = new meta_info_t(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
        se.insert(mi);
    }
    
    // dump test
    {
        // dump in normal tree
        std::vector<meta_info_t *> result;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        se.dump(result);
        gettimeofday(&end, NULL);
        printf("height: %d\n", se.height());
        print_interval(start, end);
        printf("%d\n", result.size());

        printf("\n");

        // dump in balance tree
        se.balance();
        result.clear();
        gettimeofday(&start, NULL);
        se.dump(result);
        gettimeofday(&end, NULL);
        printf("height: %d\n", se.height());
        print_interval(start, end);
        printf("%d\n", result.size()); 

        printf("\n");

        // dump in list
        std::vector<file_meta *> mresult;
        gettimeofday(&start, NULL);
        for(int i = 0; i < mlist->size; i++) {
            mresult.push_back((file_meta *)mlist->item[i].data);
        }
        gettimeofday(&end, NULL);
        print_interval(start, end);
        printf("%d\n", result.size()); 
        printf("\n");
    }

    
    {
        
        struct timeval start, end;
        meta_info_t low(0, 0, 1024*1024, 0, 0, 0);
        meta_info_t high(0, 0, 2024*1024, 0xffffffff, 0xffffffff, 0xffffffff);
        region_t r(low, high);

        // range find in kdtree

        std::cout << "range find in simple kdtree" << std::endl;
        std::vector<meta_info_t *> v;
        gettimeofday(&start, NULL);
        se.find_within_range(r, v);
        gettimeofday(&end, NULL);
        print_interval(start, end);
        printf("results: %u\n", v.size());
        printf("\n");


        v.clear();
        se.balance();
        printf("height after optimise: %d\n", se.height());

        std::cout << "range find in simple balance kdtree" << std::endl;
        gettimeofday(&start, NULL);
        se.find_within_range(r, v);
        gettimeofday(&end, NULL);
        print_interval(start, end);
        printf("results: %u\n", v.size());
        printf("\n");

    }
}

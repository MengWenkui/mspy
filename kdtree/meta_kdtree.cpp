#define KDTREE_DEFINE_OSTREAM_OPERATORS

// Make SURE all our asserts() are checked
#undef NDEBUG

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <limits>
#include <functional>
#include <set>

#include "../filter.h"
#include "../myutil.h"
#include "../partition.h"
#include "kdtree.hpp"
#include "skdtree.h"


struct meta_tuple {
   typedef uint32_t value_type;
   #define D 6

   value_type d[D];
   
   meta_tuple()
   {
   }
   
   meta_tuple(value_type uid, value_type gid, 
           value_type size, value_type atime,
           value_type ctime, value_type mtime) 
   {
       d[0] = uid;
       d[1]= gid;
       d[2] = size;
       d[3] = atime;
       d[4] = ctime;
       d[5] = mtime;
   }

   ~meta_tuple()
   {

   }

   inline value_type operator[](size_t const N) const 
   {
       return d[N];
   }
    
};


inline bool operator==(meta_tuple const& A, meta_tuple const& B) 
{
   for(int i = 0; i < D; i++) {
        if(A.d[i] != B.d[i]) {
            return false;
        }
   }
   return true;
}


std::ostream& operator<<(std::ostream& out, meta_tuple const& T) 
{
    out << '(';
    for(int i = 0; i < D; i++) {
        out << T.d[i] << " ";
    }
    out << ')';
}

inline uint32_t tac(meta_tuple t, size_t k)
{
    return t[k];
}


typedef KDTree::KDTree<6, meta_tuple, std::pointer_to_binary_function<meta_tuple,size_t, uint32_t> > tree_type;



void print_interval(struct timeval start, struct timeval end)
{

    uint64_t val = (end.tv_sec - start.tv_sec) * 1000000 +
        (end.tv_usec - start.tv_usec);
    printf("%lluus\n",val);
}

void print_meta_info(meta_info_t *meta)
{
    std::cout << '(';
    for(int i = 0; i < D; i++) {
        std::cout << meta->d[i] << " ";
    }
    std::cout << ')'  << std::endl;
}

file_meta *mylist_find(mylist *mlist, const meta_tuple &c)
{
    for(int i = 0; i < mlist->size; i++) {
        file_meta *m = (file_meta *)mlist->item[i].data;
        if(m->uid == c[0] && m->gid == c[1] && 
           m->size == c[2] && m->atime == c[3] && 
            m->ctime == c[4] && m->mtime == c[5]) {
            return m;
        }
    }
    return NULL;
}


int mylist_range_find(mylist *mlist, const meta_tuple &c1, const meta_tuple &c2)
{
    int count = 0;
    for(int i = 0; i < mlist->size; i++) {
        file_meta *m = (file_meta *)mlist->item[i].data;
        if(m->uid >= c1[0] && m->uid <= c2[0] &&
           m->gid >= c1[1] && m->uid <= c2[1] &&
           m->size >= c1[2] && m->size <= c2[2] &&
           m->atime >= c1[3] && m->atime <= c2[3] &&
           m->ctime >= c1[4] && m->ctime <= c2[4] &&
           m->mtime >= c1[5] && m->mtime <= c2[5]) {
            count++;
        }
    }

    return count;
}

int main(int argc, char *argv[])
{
    if(2 != argc) {
        printf("./meta_kdtree metapath\n");
        return -1;
    }


    tree_type src(std::ptr_fun(tac));
    skdtree_t se(6);

    mylist *mlist = meta_read(argv[1]);
    printf("partiton size = %d\n", mlist->size);


    // keep first, middle last file
    file_meta *m = NULL;
    m = (file_meta *)mlist->item[0].data;
    meta_tuple first(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
    meta_info_t first_mi(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
    m = (file_meta *)mlist->item[mlist->size/2].data;
    meta_tuple mid(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
    meta_info_t mid_mi(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
    m = (file_meta *)mlist->item[mlist->size-1].data;
    meta_tuple last(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
    meta_info_t last_mi(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);

 
    std::cout << "first: " << first << std::endl;
    std::cout << "mid: " << mid << std::endl;
    std::cout << "last: " << last << std::endl;

//    print_meta_info(&first_mi);
//    print_meta_info(&mid_mi);
//    print_meta_info(&last_mi);

    // insert every file's metadata to kdtree
    for(int i = 0; i < mlist->size; i++) {
        file_meta *m = (file_meta *)mlist->item[i].data;
        meta_tuple c(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
        src.insert(c);
    }

    src.optimise();

    // insert every file's metadata to simple kdtree
    for(int i = 0; i < mlist->size; i++) {
        file_meta *m = (file_meta *)mlist->item[i].data;
        meta_info_t *mi = new meta_info_t(m->uid, m->gid, m->size, m->atime, m->ctime, m->mtime);
        se.insert(mi);
    }

    printf("height of simple kdtree: %d\n", se.height());
   // se.balance();
   // printf("height after optimise: %d\n", se.height());
  // / print_meta_info(se.root->meta);

    struct timeval start, end;

    {
        std::cout << "find in kdtree" << std::endl;
        tree_type::const_iterator iter;
        
        gettimeofday(&start, NULL);
        iter = src.find_exact(first);
        gettimeofday(&end, NULL);
        print_interval(start, end);

        iter = src.find_exact(mid);
        gettimeofday(&end, NULL);
        print_interval(start, end);

        gettimeofday(&start, NULL);
        iter = src.find_exact(last);
        gettimeofday(&end, NULL);
        print_interval(start, end);

    }

    {
        std::cout << "find in list" << std::endl;
        file_meta *res = NULL;

        gettimeofday(&start, NULL);
        res = mylist_find(mlist, first);
        gettimeofday(&end, NULL);
        print_interval(start, end);

        gettimeofday(&start, NULL);
        res = mylist_find(mlist, mid);
        gettimeofday(&end, NULL);
        print_interval(start, end);

        gettimeofday(&start, NULL);
        res = mylist_find(mlist, last);
        gettimeofday(&end, NULL);
        print_interval(start, end);
    }

    {
        std::cout << "find in simple kdtree" << std::endl;
        meta_info_t *meta;

        gettimeofday(&start, NULL);
        meta = se.find_exact(first_mi);
        gettimeofday(&end, NULL);
        print_interval(start, end);

        gettimeofday(&start, NULL);
        meta = se.find_exact(mid_mi);
        gettimeofday(&end, NULL);
        print_interval(start, end);
        
        gettimeofday(&start, NULL);
        meta = se.find_exact(last_mi);
        gettimeofday(&end, NULL);
        print_interval(start, end);
    }

    meta_tuple c1(0, 0, 0, 0, 0, 0);
    meta_tuple c2(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    meta_info_t low(0, 0, 0, 0, 0, 0);
    meta_info_t high(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
    region_t r(low, high);


    {
        {
            std::cout << "range find in kdtree no recursively" << std::endl;
            std::vector<meta_tuple> v;
            gettimeofday(&start, NULL);
            src.find_within_range_norec(c1, c2, std::back_inserter(v));
            // src.find_within_range(c1, c2, o);
            gettimeofday(&end, NULL);
            print_interval(start, end);
            printf("results: %u\n", v.size());
        }

        {
            std::cout << "range find in kdtree recursively" << std::endl;
            std::vector<meta_tuple> v;
            gettimeofday(&start, NULL);
            src.find_within_range(c1, c2, std::back_inserter(v));
            gettimeofday(&end, NULL);
            print_interval(start, end);
            printf("results: %u\n", v.size());
        }

        {
            std::cout << "range count in kdtree recursively" << std::endl;
            gettimeofday(&start, NULL);
            int res = src.count_within_range(c1, c2);
            gettimeofday(&end, NULL);
            print_interval(start, end);
            printf("results: %u\n", res);
        }
    }

    {
        std::cout << "range find in simple kdtree" << std::endl;
        std::vector<meta_info_t *> v;
        gettimeofday(&start, NULL);
        se.find_within_range(r, v);
        gettimeofday(&end, NULL);
        print_interval(start, end);
        printf("results: %u\n", v.size());

    }

    {
        std::cout << "range find in simple kdtree norecursively" << std::endl;
        std::vector<meta_info_t *> v;
        gettimeofday(&start, NULL);
        se.find_within_range_norec2(r, v);
        gettimeofday(&end, NULL);
        print_interval(start, end);
        printf("results: %u\n", v.size());

    }


    {
        std::cout << "range find in list" << std::endl;
        gettimeofday(&start, NULL);
        int res = mylist_range_find(mlist, c1, c2);
        gettimeofday(&end, NULL);
        print_interval(start, end);
        printf("results: %u\n", res);

    }


    return 0;
}




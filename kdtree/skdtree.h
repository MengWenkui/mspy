#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <iostream>


struct meta_info_t {
    #define D 6
    uint32_t d[D];
    meta_info_t(uint32_t uid, uint32_t gid, 
            uint32_t size, uint32_t atime,
            uint32_t ctime, uint32_t mtime) 
    {
        /*
        d[0] = uid;
        d[1] = gid;
        d[2] = size;
        d[3] = atime;
        d[4] = ctime;
        d[5] = mtime;
        */

        d[0]= size;
        d[1]= atime;
        d[2] = ctime;
        d[3] = mtime;
        d[4] = uid;
        d[5] = gid;
    }

    bool compare(const meta_info_t &that) const
    {
        for(int i = 0; i < D; i++) {
            if(d[i] != that.d[i]) {
                return false;
            }
        }
        return true;
    }
};

inline bool operator==(meta_info_t const& A, meta_info_t const& B) 
{
   for(int i = 0; i < D; i++) {
        if(A.d[i] != B.d[i]) {
            return false;
        }
   }
   return true;
}

/*
std::ostream& operator<<(std::ostream& out, meta_info_t const& T) 
{
    out << '(';
    for(int i = 0; i < D; i++) {
        out << T.d[i] << " ";
    }
    out << ')';
}

*/

struct region_t {
    meta_info_t low;
    meta_info_t high;
    
    region_t(const meta_info_t& l, const meta_info_t& h):
        low(l), high(h)
    {
    }
    
    void set_low_bound(int level, int K, int val) 
    {
        low.d[level%K] = val;
    }
    
    void set_high_bound(int level, int K, int val)
    {
        high.d[level%K] = val; 
    }


    bool intersect_with(const region_t &that) const
    {
        for(int i = 0; i < D; i++) {
            if(that.low.d[i] > high.d[i] || that.high.d[i] < low.d[i]) {
                return false;
            }
        }
        return true;
    }

    bool encloses(const meta_info_t &that) const
    {
         for(int i = 0; i < D; i++) {
            if(that.d[i] > high.d[i] || that.d[i] < low.d[i]) {
                return false;
            }
        }
        return true;
    }

};

struct node_cmp_t {
    int k;
    node_cmp_t(int k) 
    {
        this->k = k;
    }

    bool operator()(meta_info_t *first, meta_info_t *second) const
    {
        return first->d[k] < second->d[k]; // not <=, attention
    }
};

struct skdtree_node_t {
    meta_info_t *meta;
    struct skdtree_node_t *left;
    struct skdtree_node_t *right;
    skdtree_node_t()
    {
        meta = NULL;
        left = NULL;
        right = NULL;
    }
};

struct skdtree_t {
    typedef std::vector<meta_info_t *> result_type;
public:
    skdtree_t(int k)
    {
        root = NULL;
        K = k;
    }
    void insert(meta_info_t *meta);
    meta_info_t* find_exact(const meta_info_t& target);
    void find_within_range(const region_t &r, result_type &result);
    void find_within_range_norec(const region_t &r, result_type &result);
    int height();
    void dump(result_type &result);
    void clear();
    void optimise();
    void balance();

    skdtree_node_t *root;
    int K;
  
    int _height(skdtree_node_t *node);
    void _insert(skdtree_node_t *node, meta_info_t *node, int level);
    meta_info_t* _find_exact(skdtree_node_t *node, const meta_info_t& target, int level);
    void _find_within_range(skdtree_node_t *node, const region_t &r, 
            const region_t &b, int level, result_type &result);
    void _find_within_range_norec(skdtree_node_t *node, const region_t &r, 
            result_type &result);
    void _dump(skdtree_node_t *node, result_type &result);
    void _optimise(result_type::iterator begin, result_type::iterator end, int level);
    void _balance(skdtree_node_t** node, result_type::iterator begin, result_type::iterator end, int level);
};

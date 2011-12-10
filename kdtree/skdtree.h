#include <stdio.h>
#include <stdint.h>
#include <vector>


struct meta_info_t {
    #define D 6
    uint32_t d[D];
    meta_info_t(uint32_t uid, uint32_t gid, 
            uint32_t size, uint32_t atime,
            uint32_t ctime, uint32_t mtime) 
    {
        d[0] = uid;
        d[1] = gid;
        d[2] = size;
        d[3] = atime;
        d[4] = ctime;
        d[5] = mtime;
    }

    bool compare(const meta_info_t &that) const
    {
        for(int i = 0; i < D; i++) {
            if(d[0] != that.d[0]) {
                return false;
            }
        }
        return true;
    }
};

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
private:
    skdtree_node_t *root;
    int K;
    void _insert(skdtree_node_t *node, meta_info_t *node, int level);
    meta_info_t* _find_exact(skdtree_node_t *node, const meta_info_t& target, int level);
    void _find_within_range(skdtree_node_t *node, const region_t &r, 
            const region_t &b, int level, result_type &result);
};

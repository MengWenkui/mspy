#include "skdtree.h"

void skdtree_t::insert(meta_info_t *meta)
{
    if(NULL == root) {
        root = new skdtree_node_t();
        root->meta = meta;
        return;
    }

    _insert(root, meta, 0);
}

void skdtree_t::_insert(skdtree_node_t *node, meta_info_t *meta, int level)
{
    if(NULL != node) {
        if(meta->d[level%K] < node->meta->d[level%K]) {
            if(NULL == node->left) {
                skdtree_node_t *new_node = new skdtree_node_t();
                new_node->meta = meta;
                node->left = new_node;
            } else {
                _insert(node->left, meta, level + 1);
            }
        } else {
            if(NULL == node->right) {
                skdtree_node_t *new_node = new skdtree_node_t();
                new_node->meta = meta;
                node->right = new_node;
            } else {
                _insert(node->right, meta, level + 1);
            }
        }
    }
}


meta_info_t* skdtree_t::find_exact(const meta_info_t& target)
{
    if(NULL == root) {
        return NULL;
    } 

    return _find_exact(root, target, 0);
}

meta_info_t* skdtree_t::_find_exact(skdtree_node_t *node, const meta_info_t& target, int level)
{
    if(true == node->meta->compare(target)) {
        return node->meta;
    } else if(target.d[level%K] < node->meta->d[level%K]) {
        if(NULL != node->left) {
            return _find_exact(node->left, target, level + 1); 
        }
    } else {
        if(NULL != node->right) {
            return _find_exact(node->right, target, level + 1);
        }
    }

    return NULL;
}


void skdtree_t::find_within_range(const region_t &r, result_type &result)
{
    if(NULL == root) {
        return;
    }

    region_t b(r);
    _find_within_range(root, r, b, 0, result);
}

void skdtree_t::_find_within_range(skdtree_node_t *node, const region_t& r, 
            const region_t &b, int level, result_type &result)
{
    if(true == r.encloses(*(node->meta))) {
        result.push_back(node->meta);   
    }

    if(NULL != node->left) {
        region_t nb(b);
        nb.set_high_bound(level, K, node->meta->d[level%K]);
        if(r.intersect_with(nb)) {
            _find_within_range(node->left, r, nb, level + 1, result);
        }
    }

    if(NULL != node->right) {
        region_t nb(b);
        nb.set_low_bound(level, K, node->meta->d[level%K]);
        if(r.intersect_with(nb)) {
            _find_within_range(node->right, r, nb, level + 1, result);
        }
    }
}

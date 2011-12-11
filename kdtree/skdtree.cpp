#include "skdtree.h"
#include <algorithm>

int skdtree_t::height()
{
    if(NULL == root) {
        return 0;
    }
    return _height(root);
}

int skdtree_t::_height(skdtree_node_t *node) 
{
    if(NULL == node) {
        return 0;
    }

    int lh = _height(node->left);
    int rh = _height(node->right);
    return (lh > rh? lh: rh) + 1;
}

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

void skdtree_t::dump(result_type &result)
{
    if(NULL != root) {
        _dump(root, result);
    }
}

void skdtree_t::_dump(skdtree_node_t *node, result_type &result)
{
    if(NULL != node) {
        result.push_back(node->meta);
        _dump(node->left, result);
        _dump(node->right, result);
        delete node;
    }
}

void skdtree_t::clear()
{
    root = NULL;
}

void skdtree_t::optimise()
{
    result_type result;
    dump(result);
    clear();
    _optimise(result.begin(), result.end(), 0);   
}

void skdtree_t::_optimise(result_type::iterator begin,
        result_type::iterator end, int level)
{
    if(begin == end)  {
        return;
    }
    node_cmp_t comp(level%K);
    result_type::iterator mid = begin + (end - begin) / 2;
    std::nth_element(begin, mid, end, comp);
    insert(*mid);
    if(begin != mid) {
        _optimise(begin, mid, level + 1);
    }
    if(mid + 1 != end) {
        _optimise(mid + 1, end, level + 1);
    }
}


void skdtree_t::balance()
{
    result_type result;
    dump(result);
    clear();
    _balance(&root, result.begin(), result.end(), 0);   
}

void skdtree_t::_balance(skdtree_node_t** node, result_type::iterator begin,
        result_type::iterator end, int level)
{
    if(begin == end)  {
        return;
    }
    node_cmp_t comp(level%K);
    result_type::iterator mid = begin + (end - begin) / 2;
    std::nth_element(begin, mid, end, comp);
    *node = new skdtree_node_t();
    (*node)->meta = *mid;
    if(begin != mid) {
        _balance(&((*node)->left), begin, mid, level + 1);
    }
    if(mid + 1 != end) {
        _balance(&((*node)->right), mid + 1, end, level + 1);
    }
}



#ifndef TREE_H
#define TREE_H
#include "attribute.h"
#include <unordered_map>

class Branch{
public:
    Branch(size_t n_classes,  std::vector<Attribute*> attributes,
           bitmask_t* selected_rows, long * classes);
    float get_entropy();
    std::unordered_map<long,Branch*>* children;
    int split();
    bool is_leaf;
    Attribute *split_attribute;
    long decision;
    ~Branch();
private:
    size_t n_classes;
    std::vector<Attribute*> attributes;
    long *classes;
    bitmask_t* selected_rows;
};

#endif

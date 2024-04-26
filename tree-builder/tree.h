#ifndef TREE_H
#define TREE_H
#include "attribute.h"
#include <unordered_map>
class Branch{
public:
    Branch(size_t n_classes, Attribute* attribute,
           bitmask_t* selected_rows, long * classes);
    float get_entropy();
    std::unordered_map<long,Branch*>* children;
private:
    long *classes;
    size_t n_classes;
    Attribute* attribute;
    bitmask_t* selected_rows;
    int split();
};

#endif

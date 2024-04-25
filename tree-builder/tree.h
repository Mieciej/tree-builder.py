#ifndef TREE_H
#define TREE_H
#include "feature.h"
class branch{
public:
    branch(size_t n_classes, bitmask_t* selected_features,
           bitmask_t* selected_rows, long * classes);
    float get_entropy();
private:
    long *classes;
    size_t n_classes;
    bitmask_t* selected_features;
    bitmask_t* selected_rows;
};

#endif

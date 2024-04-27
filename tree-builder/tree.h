#ifndef TREE_H
#define TREE_H
#include "attribute.h"
#include <unordered_map>

template <typename T,typename C>
class Branch{
public:
    Branch(size_t n_classes,  std::vector<Attribute<T,C>*> attributes,
           bitmask_t* selected_rows, C * classes);
    float get_entropy();
    std::unordered_map<T,Branch*>* children;
    int split();
    bool is_leaf;
    Attribute<T,C> *split_attribute;
    C decision;
private:
    size_t n_classes;
    std::vector<Attribute<T,C>*> attributes;
    C *classes;
    bitmask_t* selected_rows;
};

#endif

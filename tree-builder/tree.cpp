#include "tree.h"
#include <iostream>
#include <unordered_map>
#include <cmath>
Branch::Branch(size_t n_classes, Attribute *attribute,
               bitmask_t *selected_rows, long * classes) 
    : n_classes(n_classes), attribute(attribute),
    selected_rows(selected_rows), classes(classes) {
}

float
Branch::get_entropy(){
    float ret = 0;
    std::unordered_map<long, size_t> counter;
    for (size_t i =0; i< n_classes; i++){
        if( !(selected_rows[i/64] & (1 << i))) {
            continue;
        }
        if(!counter.count(i)){
            counter[classes[i]] = 0;
        }
        counter[classes[i]]++;
    }
    for (size_t i =0; i< n_classes; i++){
        if( !(selected_rows[i/64] & (1 << i))) {
            continue;
        }
        ret -= (float)counter[i]/(float)n_classes *
            log((float)counter[i]/(float)n_classes)/log(2.0f);
        std::cout<< ret <<std::endl;
    }
    return ret;
}

int
Branch::split(){
    children = new std::unordered_map<long, Branch*>();
    for (size_t i =0; i< n_classes; i++){
        if( !(selected_rows[i/64] & (1 << i))) {
            continue;
        }
        //if(!counter.count(i)){
            //children[classes[i]] = 
        //}
    }
    return 0;
};

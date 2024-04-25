#include "tree.h"
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <cmath>

branch::branch(size_t n_classes, bitmask_t *selected_features,
               bitmask_t *selected_rows, long * classes) 
    : n_classes(n_classes), selected_features(selected_features),
    selected_rows(selected_rows), classes(classes) {
}

float
branch::get_entropy(){
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
        ret -= (float)counter[i]/(float)n_classes* log((float)counter[i]/(float)n_classes)/log(2.0f);
        std::cout<< ret <<std::endl;
    }
    return ret;
}

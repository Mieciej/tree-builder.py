#include "tree.h"
#include <iostream>
#include <set>
#include <unordered_map>
#include <cmath>
Branch::Branch(size_t n_rows, std::vector<Attribute*> attributes,
               bitmask_t *selected_rows, long * classes) 
    : n_classes(n_rows),attributes(attributes),
    classes(classes), selected_rows(selected_rows) {
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
Branch::split(Attribute* attribute){
    children = new std::unordered_map<long, Branch*>();
    std::unordered_map<long,bitmask_t*> rows; 
    std::set<long> unique_values; 
    std::vector<Attribute *> selected_attributes;
    size_t counter_rows = 0;
    for (size_t i =0; i< n_classes; i++){
        if( !(selected_rows[i/64] & (1 << i))) {
            continue;
        }
        counter_rows++;
        long att = attribute->values->at(i)->first;
        unique_values.insert(att);
        if(!rows.count(i)){
            rows[att] = 
                new bitmask_t[n_classes/sizeof(bitmask_t)+1]{};
        }
        rows[att][i] |= (1 << i);
    }
    for(auto c : attributes){
        if(attribute == c){
            continue;
        }
        selected_attributes.push_back(c);

    }
    for (auto c : unique_values){
        Branch * child =  
            new Branch(counter_rows,selected_attributes,rows[c],classes);
        (*children)[c] = child;
    }
    return 0;
};

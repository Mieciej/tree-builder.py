#include "feature.h"
#include <unordered_map>
#include <cmath>
#include<iostream>
feature::feature(size_t n_values) {
    values = new std::vector<std::pair<long,long>*>();
    values->reserve(n_values);
    n_val = n_values;

}
float
feature::get_entropy(bitmask_t * chosen_rows){

    std::unordered_map<long,std::unordered_map<long,unsigned long>> counter;
    size_t n_chosen_rows=0;
    for ( size_t i = 0; i < n_val; i++){
        if (!(chosen_rows[i/64] & ( 1 << i)) ) {
            continue;
        }
        n_chosen_rows++;
        if(!counter.count(values->at(i)->first) && 
                          !counter[values->at(i)->first][values->at(i)->second]){
            std::unordered_map<long,unsigned long>  &c
                = counter[values->at(i)->first];
            c[values->at(i)->second] = 1;
            continue;
        }
        
        std::unordered_map<long,unsigned long>  &c =
            counter[values->at(i)->first];
        c[values->at(i)->second] += 1;
    }
    float ret = 0;
    for (auto i = counter.begin(); i!= counter.end(); i++){
        size_t n = 0;
        float ent = 0; 
        for (auto j = i->second.begin(); j != i->second.end(); j++)
        {
            n +=j->second;
        }

        for (auto j = i->second.begin(); j != i->second.end(); j++){
            ent -= (log((float)j->second/(float)n)/log(2.0));
            std::cout << log((float)j->second/(float)n)/log(2.0)<<std::endl;
        }
        ret += (float)n/(float)n_chosen_rows * ent;
        
    }
    return ret;

}

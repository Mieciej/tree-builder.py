#include "attribute.h"
#include <iostream>
#include <unordered_map>
#include <cmath>
Attribute::Attribute(size_t n_values, long label) {
    values = new std::vector<std::pair<long,long>*>();
    values->reserve(n_values);
    n_val = n_values;
    this->label = label;

}

float
Attribute::get_entropy(bitmask_t * chosen_rows){

    std::cout << "Attribute: "<< label <<std::endl; 
    // value:{class: count}
    std::unordered_map<long,std::unordered_map<long,unsigned long>> counter;
    size_t n_chosen_rows=0;
    for ( size_t row = 0; row < n_val; row++){
        if (!(chosen_rows[row/64] & ( 1 << row)) ) {
            continue;
        }
        n_chosen_rows++;
        if(!counter.count(values->at(row)->first) && 
        !counter[values->at(row)->first][values->at(row)->second]){

            std::unordered_map<long,unsigned long>  &unique_class
                = counter[values->at(row)->first];
            unique_class[values->at(row)->second] = 1;
            continue;
        }
        std::unordered_map<long,unsigned long>  &c =
            counter[values->at(row)->first];
        c[values->at(row)->second] += 1;
    }
    float ret = 0;
    for (auto unique_value : counter){
        size_t n_occurrences = 0;
        float ent = 0; 
        for (auto unique_class = unique_value.second.begin(); 
        unique_class != unique_value.second.end(); unique_class++){\
            n_occurrences +=unique_class->second;
        }
        for (auto unique_class = unique_value.second.begin(); 
        unique_class != unique_value.second.end(); unique_class++){
            if (unique_class->second == 0){
                continue;
            }
            float t = (float)unique_class->second/n_occurrences*
                log2((float)unique_class->second/n_occurrences);
            ent -= t;
            std::cout <<"\tClass: " << unique_class->first<<
                " has occured " << unique_class->second <<"/"<<n_occurrences<<
                " and has entropy " << -t<<std::endl;
        }
        std::cout <<"\tValue: " << unique_value.first <<" ent: "<<ent 
            << std::endl << std::endl;
        ret += (float)n_occurrences/(float)n_chosen_rows * ent;
    }
    std::cout << "\tFinal conditional entropy: " <<ret << std::endl;
    return ret;

}
Attribute::~Attribute(){
    for(auto c : *values){
        delete(c);
    }
    delete(values);
}

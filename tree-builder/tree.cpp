#include "tree.h"
#include <cassert>
#include <set>
#include <unordered_map>
#include <cmath>
#include <bitset>
Branch::Branch(size_t n_rows, std::vector<Attribute*> attributes,
               bitmask_t *selected_rows, long * classes) 
    : n_classes(n_rows),attributes(attributes),
    classes(classes), selected_rows(selected_rows) {
    long last_seen_class;
    bool has_seen_class = false;
    is_leaf = true;
    for (size_t i =0; i< n_classes; i++){
        if( !(selected_rows[i/64] & (1 << i))) {
            continue;
        }
        if (!has_seen_class){
            last_seen_class = classes[i];
            has_seen_class = true;
            continue;
        }
        if (last_seen_class != classes[i]){
            is_leaf = false;
            break;
        }
    }

    if(is_leaf) {
        std::bitset<64> y(selected_rows[0]);
    }
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
    }
    return ret;
}

int
Branch::split(){
    if (is_leaf) return 1;
    Attribute *attribute = nullptr;
    children = new std::unordered_map<long, Branch*>();
    std::unordered_map<long,bitmask_t*> rows; 
    std::set<long> unique_values; 
    std::vector<Attribute *> selected_attributes;
    size_t counter_rows = 0;
    float top_ent = INFINITY;
    for ( Attribute *att : attributes) {
        float curr_ent = att->get_entropy(selected_rows);
        if ( curr_ent < top_ent ) {
            attribute = att;
            top_ent = curr_ent;
        }
    }
    assert(((void)"NO ATTRIBUTE FOUND",attribute !=nullptr));

    split_attribute = attribute;
    for (size_t i =0; i< n_classes; i++){
        if( !(selected_rows[i/64] & (1 << i))) {
            continue;
        }
        counter_rows++;
        long att = attribute->values->at(i)->first;
        unique_values.insert(att);
        if(!rows.count(att)){
            rows[att] = 
                new bitmask_t[n_classes/64+1]{};

        }
        rows[att][i/64] |= (1<<i);

    }
    assert(((void)"DID NOT FIND UNIQUE VALUES",unique_values.size()==0));  
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

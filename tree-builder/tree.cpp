#include "tree.h"
#include <cassert>
#include <unordered_map>
#include <cmath>
#include <iostream>
#include <bitset>
#include <set>
Branch::Branch(size_t n_rows, std::vector<Attribute*> attributes,
               bitmask_t *selected_rows, long * classes) 
    : n_classes(n_rows),attributes(attributes),
    classes(classes), selected_rows(selected_rows) {
    children  = nullptr;
    std::unordered_map<long, size_t> counter;
    size_t top_size=0;
    if (attributes.size() == 0){

        for (size_t row =0; row< n_classes; row++){
            if( !(selected_rows[row/64] & (1 << row))) {
                continue;
            }
            if(!counter.count(classes[row])){
                counter[classes[row]] = 0;
            }
            counter[classes[row]]++;
            if( counter[classes[row]] > top_size){
                decision = classes[row];
            }
        }
        is_leaf = true;
        
        std::cout << "\tI am a leaf with majority decision: " 
            <<decision << std::endl; 
        return;
    }


    long last_seen_class = 0;
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

    if(is_leaf){

        decision = last_seen_class;
        std::cout << "\tI am a leaf with decision: " 
            <<decision << std::endl; 
    }

}

float
Branch::get_entropy(){
    float ret = 0;
    std::unordered_map<long, size_t> counter;
    size_t n_selected_rows = 0;
    for (size_t row =0; row< n_classes; row++){
        if( !(selected_rows[row/64] & (1 << row))) {
            continue;
        }
        n_selected_rows++;
        if(!counter.count(row)){
            counter[classes[row]] = 0;
        }
        counter[classes[row]]++;
    }
    for (size_t row =0; row< n_classes; row++){
        if( !(selected_rows[row/64] & (1 << row))) {
            continue;
        }
        if(counter[classes[row]] == 0) {
            continue;
        }
        ret -= (float)counter[classes[row]]/(float)n_selected_rows *
            log2((float)counter[classes[row]]/(float)n_selected_rows);
    }
    return ret;
}
int
Branch::split(){
    if (is_leaf) return 1;

    children = new std::unordered_map<long, Branch*>();
    std::unordered_map<long,bitmask_t*> passed_on_rows; 
    std::set<long> unique_values; 
    std::vector<Attribute *> selected_attributes;

    Attribute *best_attribute = nullptr;
    float top_info_gain = -INFINITY;
    float ent_class = get_entropy();

    for ( Attribute *att : attributes) {
        float info_gain = ent_class -  att->get_entropy(selected_rows);
        if ( info_gain > top_info_gain ) {
            best_attribute = att;
            top_info_gain = info_gain;
        }
    }
    std::cout << "Splitting on attribute: " << best_attribute->label<<
        " with information gain: " <<top_info_gain<<std::endl;
    split_attribute = best_attribute;
    for (size_t row =0; row< n_classes; row++){
        if( !(selected_rows[row/64] & (1 << row))) {
            continue;
        }
        long att_value = best_attribute->values->at(row)->first;
        unique_values.insert(att_value);
        if(!passed_on_rows.count(att_value)){
            passed_on_rows[att_value] = 
                new bitmask_t[n_classes/64+1]{};

        }
        passed_on_rows[att_value][row/64] |= (1<<row);

    }
    for(auto c : attributes){
        if(best_attribute == c){
            continue;
        }
        selected_attributes.push_back(c);

    }
    for (auto value : unique_values){
        std::cout << "\tValue: " << value 
            <<", selected "<< n_classes <<" rows: "<< 
            std::bitset<16>(passed_on_rows[value][0]) <<std::endl;
        Branch * child =  
            new Branch(n_classes,selected_attributes,passed_on_rows[value],classes);
        (*children)[value] = child;
    }
    return 0;
};

void expand_tree(Branch* b){
    Branch *curr = b;
    if (!(curr->split())){
        for(auto child : *curr->children){
            expand_tree(child.second);
        }
    }
    if(curr->is_leaf){
        return;
    }
}

Branch::~Branch(){
    
    if(children!=nullptr){
        for(auto c : *children){
            delete(c.second);
        }
        delete(children);

    } 
    delete(selected_rows);


}


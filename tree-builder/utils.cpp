#include "utils.h"
#include <iostream>
extern "C" void print_tree_helper(const std::string& prefix, const Branch* node,
                                  bool not_last,
                                  long value,std::unordered_map<long,std::string> &dictionary,
                                  bool first)
{
    if (node != nullptr)
    {
        std::cout << prefix;

        std::cout << (not_last ? "├": "└");
        std::string print_value;
        if(!first) print_value= dictionary[value];
        else print_value="─";
        std::cout << print_value <<"──";
        if(!node->is_leaf) {
            std::cout << node->split_attribute->label << std::endl;
        } else {
            std::cout << dictionary[node->decision]<<std::endl;
            return;
        }
        size_t i =0;
        for (auto c: *node->children)
        {
            bool nl =  i != node->children->size()-1;
            std::string bonus = "";
            for (size_t i = 0; i<print_value.size()-1; i++) {
                bonus+=" ";
            }
            print_tree_helper(prefix + (not_last ? "│    " : "     ") + bonus,  c.second,nl , c.first,dictionary, false);
            ++i;
        }
    }
}

extern "C" void print_tree(const Branch* node, long value, std::unordered_map<long,std::string> &dictionary)
{
    print_tree_helper("", node, false, value, dictionary,true);
}


#ifndef UTILS_H_
#define UTILS_H_
#include "tree.h"
#include "attribute.h"
#include <string>
extern "C" void print_tree(const Branch* node, long value,
                        std::unordered_map<long, std::string> &dictionary);
extern "C" void print_tree_helper(const std::string& prefix, const Branch* node,
                                  bool not_last, long value,
                                  std::unordered_map<long, std::string> &dictionary,
                                  bool first);
#endif

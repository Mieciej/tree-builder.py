#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <stdlib.h>
#include <utility>
#include <vector>
typedef unsigned long long int bitmask_t ;
template <typename T,typename C>

class Attribute {
private:
  size_t n_val;
public:
  Attribute(size_t n_values, long label);
  std::vector<std::pair<T,C>*> *values;
  float get_entropy(bitmask_t * chosen_rows);
  long label;
};

#endif 

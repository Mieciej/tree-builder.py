#ifndef FEATURE_H
#define FEATURE_H
#include <stdlib.h>
#include <utility>
#include <vector>

typedef unsigned long long int bitmask_t ;

class feature {
private:
  size_t n_val;
public:
  feature(size_t n_values);
  std::vector<std::pair<long,long>*> *values;
  float get_entropy(bitmask_t * chosen_rows);
};

#endif // !FEATURE_H

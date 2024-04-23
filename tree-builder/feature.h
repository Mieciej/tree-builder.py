#ifndef FEATURE_H
#define FEATURE_H
#include <stdlib.h>
#include <utility>
#include <vector>

class feature {
public:
  feature(size_t n_values);
  std::vector<std::pair<long,long>*> *values;
};

#endif // !FEATURE_H

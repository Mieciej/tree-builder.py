#include "feature.h"

feature::feature(size_t n_values) {
    values = new std::vector<std::pair<long,long>*>();
    values->reserve(n_values);

}

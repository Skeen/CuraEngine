#ifndef REMOVE_UTILS_H
#define REMOVE_UTILS_H

#include <algorithm>

// Remove all elements from 'c', where the predicate returns true
template<typename Collection, typename Predicate>
void remove_if(Collection c, Predicate l)
{
    c.erase(std::remove_if(std::begin(c), std::end(c), l), std::end(c));
}


#endif //REMOVE_UTILS_H

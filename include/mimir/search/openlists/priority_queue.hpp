#ifndef MIMIR_SEARCH_OPENLISTS_PRIORITY_QUEUE_HPP_
#define MIMIR_SEARCH_OPENLISTS_PRIORITY_QUEUE_HPP_

#include "template.hpp"

#include <queue>

namespace mimir
{

/**
 * Derived ID class.
 * 
 * Define name and template parameters of your own implementation.
*/
template<typename T>
struct PriorityQueue : public OpenListBaseTag { };


/**
 * Implementation class
*/
template<typename T>
class OpenList<PriorityQueue<T>> : public OpenListBase<OpenList<PriorityQueue<T>>> {
    // Implement configuration specific functionality.
private:
    std::priority_queue<std::pair<double, T>, std::vector<std::pair<double, T>>, std::greater<std::pair<double, T>>> priority_queue_;

    void insert_impl(const T& item, double priority) {
        priority_queue_.emplace(priority, item);
    }

    T pop_impl() {
        T item = priority_queue_.top().second;
        priority_queue_.pop();
        return item;
    }

    std::size_t size_impl() {
        return priority_queue_.size();
    }

    friend class OpenListBase<OpenList<PriorityQueue<T>>>;
};


/**
 * Type traits
*/
template<typename T>
struct TypeTraits<PriorityQueue<T>> {
    using ValueType = T;
};

template<typename T>
struct TypeTraits<OpenList<PriorityQueue<T>>> {
    using ValueType = T;
};



}  // namespace mimir

#endif  // MIMIR_SEARCH_OPENLISTS_PRIORITY_QUEUE_HPP_

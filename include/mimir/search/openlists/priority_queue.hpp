#ifndef MIMIR_SEARCH_OPENLISTS_PRIORITY_QUEUE_HPP_
#define MIMIR_SEARCH_OPENLISTS_PRIORITY_QUEUE_HPP_

#include "mimir/search/openlists/interface.hpp"

#include <queue>

namespace mimir
{

/**
 * Derived ID class.
 *
 * Define name and template parameters of your own implementation.
 */
template<typename T>
struct PriorityQueueTag : public OpenListTag
{
};

/**
 * Implementation class
 */
template<typename T>
class OpenList<OpenListDispatcher<PriorityQueueTag<T>>> : public IOpenList<OpenList<OpenListDispatcher<PriorityQueueTag<T>>>>
{
    // Implement configuration specific functionality.
private:
    std::priority_queue<std::pair<double, T>, std::vector<std::pair<double, T>>, std::greater<std::pair<double, T>>> priority_queue_;

    /* Implement IOpenList interface */
    friend class IOpenList<OpenList<OpenListDispatcher<PriorityQueueTag<T>>>>;

    void insert_impl(const T& item, double priority) { priority_queue_.emplace(priority, item); }

    T pop_impl()
    {
        T item = priority_queue_.top().second;
        priority_queue_.pop();
        return item;
    }

    std::size_t size_impl() { return priority_queue_.size(); }
};

}  // namespace mimir

#endif  // MIMIR_SEARCH_OPENLISTS_PRIORITY_QUEUE_HPP_

#ifndef MIMIR_SEARCH_EVENT_HANDLERS_MINIMAL_HPP_
#define MIMIR_SEARCH_EVENT_HANDLERS_MINIMAL_HPP_

#include "mimir/common/printers.hpp"
#include "mimir/common/translations.hpp"
#include "mimir/search/event_handlers/interface.hpp"
#include "mimir/search/statistics.hpp"

namespace mimir
{

/**
 * ID class to dispatch a specialized implementation
 */
struct MinimalEventHandlerTag : public EventHandlerTag
{
};

/**
 * Implementation class
 */
template<>
class EventHandler<EventHandlerDispatcher<MinimalEventHandlerTag>> : public EventHandlerBase<EventHandler<EventHandlerDispatcher<MinimalEventHandlerTag>>>
{
private:
    /* Implement EventHandlerBase interface */
    template<typename>
    friend class EventHandlerBase;

    template<IsActionDispatcher A, IsStateDispatcher S>
    void on_generate_state_impl(ConstView<A> action, ConstView<S> successor_state, const PDDLFactories& pddl_factories) const
    {
    }

    template<IsStateDispatcher S>
    void on_expand_state_impl(ConstView<S> state, const PDDLFactories& pddl_factories) const
    {
    }

    template<IsStateDispatcher S>
    void on_start_search_impl(ConstView<S> initial_state, const PDDLFactories& pddl_factories) const
    {
    }

    void on_end_search_impl() const
    {
        std::cout << "Num expanded states: " << this->m_statistics.get_num_expanded() << "\n"
                  << "Num generated states: " << this->m_statistics.get_num_generated() << "\n"
                  << "Search time: " << this->m_statistics.get_search_time_ms() << std::endl;
    }

    template<IsActionDispatcher A>
    void on_solved_impl(const std::vector<ConstView<A>>& ground_action_plan) const
    {
        auto plan = to_plan(ground_action_plan);
        std::cout << "Plan found with cost: " << plan.get_cost() << std::endl;
        for (size_t i = 0; i < plan.get_actions().size(); ++i)
        {
            std::cout << i + 1 << ". " << plan.get_actions()[i] << std::endl;
        }
    }

    void on_exhausted_impl() const { std::cout << "Exhausted!" << std::endl; }
};

}

#endif
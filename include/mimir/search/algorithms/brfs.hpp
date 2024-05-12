#ifndef MIMIR_SEARCH_ALGORITHMS_BRFS_HPP_
#define MIMIR_SEARCH_ALGORITHMS_BRFS_HPP_

#include "mimir/common/printers.hpp"
#include "mimir/common/translations.hpp"
#include "mimir/formalism/declarations.hpp"
#include "mimir/search/algorithms/interface.hpp"
#include "mimir/search/applicable_action_generators.hpp"
#include "mimir/search/event_handlers.hpp"
#include "mimir/search/heuristics.hpp"
#include "mimir/search/search_nodes/cost/cost.hpp"
#include "mimir/search/successor_state_generators.hpp"

#include <deque>
#include <flatmemory/flatmemory.hpp>
#include <functional>
#include <ostream>
#include <tuple>
#include <vector>

namespace mimir
{

/**
 * Specialized implementation class.
 */
class BrFsAlgorithm : public IAlgorithm
{
private:
    Problem m_problem;
    PDDLFactories& m_pddl_factories;
    std::shared_ptr<IDynamicSSG> m_state_repository;
    State m_initial_state;
    std::shared_ptr<IDynamicAAG> m_successor_generator;
    std::deque<State> m_queue;
    flat::CostSearchNodeVector m_search_nodes;
    std::shared_ptr<IEventHandler> m_event_handler;

    /// @brief Compute the plan consisting of ground actions by collecting the creating actions
    ///        and reversing them.
    /// @param view The search node from which backtracking begins for which the goal was
    ///             satisfied.
    /// @param[out] out_plan The sequence of ground actions that leads from the initial state to
    ///                      the to the state underlying the search node.
    void set_plan(const ConstCostSearchNode& view, GroundActionList& out_plan) const
    {
        out_plan.clear();
        auto cur_view = view;

        while (cur_view.get_parent_state_id() != -1)
        {
            out_plan.push_back(m_successor_generator->get_action(cur_view.get_creating_action_id()));

            cur_view = ConstCostSearchNode(this->m_search_nodes[cur_view.get_parent_state_id()]);
        }

        std::reverse(out_plan.begin(), out_plan.end());
    }

    /// @brief Creates a CostSearchNodeBuilderProxy whose attributes are default initialized.
    static auto create_default_search_node_builder()
    {
        auto builder = CostSearchNodeBuilder();
        builder.set_status(SearchNodeStatus::CLOSED);
        builder.set_g_value(-1);
        builder.set_parent_state_id(-1);
        builder.set_creating_action_id(-1);
        builder.finish();
        return builder.get_flatmemory_builder();
    }

public:
    BrFsAlgorithm(const Problem& problem,
                  PDDLFactories& pddl_factories,
                  std::shared_ptr<IDynamicSSG> state_repository,
                  std::shared_ptr<IDynamicAAG> successor_generator,
                  std::shared_ptr<IEventHandler> event_handler) :
        m_problem(problem),
        m_pddl_factories(pddl_factories),
        m_state_repository(std::move(state_repository)),
        m_initial_state(m_state_repository->get_or_create_initial_state(problem)),
        m_successor_generator(std::move(successor_generator)),
        m_search_nodes(flat::CostSearchNodeVector(create_default_search_node_builder())),
        m_event_handler(std::move(event_handler))
    {
    }

    SearchStatus find_solution(GroundActionList& out_plan) override
    {
        m_event_handler->on_start_search(this->m_initial_state, m_pddl_factories);

        auto initial_search_node = CostSearchNode(this->m_search_nodes[this->m_initial_state.get_id()]);
        initial_search_node.get_g_value() = 0;
        initial_search_node.get_status() = SearchNodeStatus::OPEN;

        const auto& goal_ground_literals = m_problem->get_goal_condition();

        auto applicable_actions = GroundActionList {};

        m_queue.emplace_back(m_initial_state);

        auto g_value = uint64_t { 0 };

        while (!m_queue.empty())
        {
            const auto state = m_queue.front();
            m_queue.pop_front();

            if (state.literals_hold(goal_ground_literals))
            {
                std::cout << "@@@ GOAL STATE @@@ " << std::endl;
                std::cout << std::make_tuple(state, std::cref(m_pddl_factories)) << std::endl;

                set_plan(ConstCostSearchNode(this->m_search_nodes[state.get_id()]), out_plan);

                m_event_handler->on_end_search();
                m_event_handler->on_solved(out_plan);

                return SearchStatus::SOLVED;
            }

            // if (state.get_id() == 36)
            //{
            //     std::cout << "@@@ FAILED GOAL TEST @@@ " << std::endl;
            //     std::cout << std::make_tuple(state, std::cref(m_pddl_factories)) << std::endl;
            //     return SearchStatus::FAILED;
            // }

            auto search_node = CostSearchNode(this->m_search_nodes[state.get_id()]);
            search_node.get_status() = SearchNodeStatus::CLOSED;

            if (static_cast<uint64_t>(search_node.get_g_value()) > g_value)
            {
                g_value = search_node.get_g_value();
                m_event_handler->on_finish_g_layer(g_value, m_state_repository->get_state_count());
                // if (g_value == 3)
                //{
                //     return SearchStatus::FAILED;
                // }
            }

            m_event_handler->on_expand_state(state, m_pddl_factories);

            this->m_successor_generator->generate_applicable_actions(state, applicable_actions);
            for (const auto& action : applicable_actions)
            {
                const auto state_count = m_state_repository->get_state_count();
                const auto& successor_state = this->m_state_repository->get_or_create_successor_state(state, action);
                // if (successor_state.get_id() == 36)
                //{
                //     std::cout << "@@@ CREATED GOAL STATE @@@ " << std::endl;
                //     std::cout << std::make_tuple(successor_state, std::cref(m_pddl_factories)) << std::endl;
                //     return SearchStatus::FAILED;
                // }
                m_event_handler->on_generate_state(action, successor_state, m_pddl_factories);

                if (state_count != m_state_repository->get_state_count())
                {
                    auto successor_search_node = CostSearchNode(this->m_search_nodes[successor_state.get_id()]);
                    successor_search_node.get_status() = SearchNodeStatus::OPEN;
                    successor_search_node.get_g_value() = search_node.get_g_value() + 1;
                    successor_search_node.get_parent_state_id() = state.get_id();
                    successor_search_node.get_creating_action_id() = action.get_id();

                    m_queue.emplace_back(successor_state);
                }
            }
        }

        m_event_handler->on_end_search();
        m_event_handler->on_exhausted();

        return SearchStatus::EXHAUSTED;
    }
};

}

#endif

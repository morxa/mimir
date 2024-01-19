#ifndef MIMIR_SEARCH_ALGORITHM_BASE_HPP_
#define MIMIR_SEARCH_ALGORITHM_BASE_HPP_

#include "config.hpp"
#include "state_base.hpp"
#include "type_traits.hpp"

#include "search_node.hpp"
#include "search_node_view.hpp"
#include "search_node_builder.hpp"
#include "grounded/state_builder.hpp"
#include "grounded/state_repository.hpp"
#include "grounded/successor_generator.hpp"
#include "lifted/state_builder.hpp"
#include "lifted/state_repository.hpp"
#include "lifted/successor_generator.hpp"

#include "../buffer/containers/vector.hpp"
#include "../common/mixins.hpp"
#include "../formalism/problem/declarations.hpp"


namespace mimir
{

enum SearchStatus {IN_PROGRESS, TIMEOUT, FAILED, SOLVED};

/// @brief CRTP based interface for a search algorithm
/// @tparam Derived
template<typename Derived>
class AlgorithmBase : public UncopyableMixin<AlgorithmBase<Derived>> {
private:
    using C = typename TypeTraits<Derived>::ConfigType;

    AlgorithmBase(const Problem& problem)
        : m_problem(problem)
        , m_state_repository(StateRepository<C>())
        , m_initial_state(m_state_repository.get_or_create_initial_state(problem))
        , m_search_nodes(AutomaticVector(Builder<SearchNode<C>>(SearchNodeStatus::CLOSED, 0, nullptr, nullptr))) { }

    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

    Problem m_problem;
    StateRepository<C> m_state_repository;
    State<C> m_initial_state;
    SuccessorGenerator<C> m_successor_generator;
    AutomaticVector<SearchNode<C>> m_search_nodes;

public:
    SearchStatus find_solution(GroundActionList& out_plan) {
        return self().find_solution_impl(out_plan);
    }
};

}  // namespace mimir

#endif  // MIMIR_SEARCH_ALGORITHM_BASE_HPP_

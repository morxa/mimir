#ifndef MIMIR_SEARCH_HEURISTIC_TEMPLATE_HPP_
#define MIMIR_SEARCH_HEURISTIC_TEMPLATE_HPP_

#include "grounded/state_view.hpp"
#include "lifted/state_view.hpp"
#include "type_traits.hpp"

#include "../common/mixins.hpp"
#include "../formalism/problem/declarations.hpp"


namespace mimir
{

/**
 * Interface class
*/
template<typename Derived, IsPlanningModeTag P>
class HeuristicBase : public UncopyableMixin<HeuristicBase<Derived, P>> {
private:
    HeuristicBase(Problem problem) : m_problem(problem) { }
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

    Problem m_problem;

public:
    [[nodiscard]] double compute_heuristic(View<State<P>> state) {
        return self().compute_heuristic_impl(state);
    }
};


/**
 * ID class.
 *
 * Derive from it to provide your own implementation of a heuristic.
*/
struct HeuristicBaseTag {};

template<class DerivedTag>
concept IsHeuristicTag = std::derived_from<DerivedTag, HeuristicBaseTag>;


/**
 * Make heuristics accept a PlanningModeTag.
 * This makes the template instiation less constraint
 * and we can use the PlanningModeTag of the actual search algorithm.
*/
template<IsHeuristicTag H, IsPlanningModeTag P>
struct HeuristicInstantiation {
    using HeuristicTag = H;
    using PlanningModeTag = P;
};

template<typename T>
concept IsHeuristicInstantiation = requires {
    typename T::HeuristicTag;
    typename T::PlanningModeTag;
};


/**
 * General implementation class.
 *
 * Spezialize it with your heuristic instantiation to provide your own implementation of a heuristic.
*/
template<IsHeuristicInstantiation T, IsPlanningModeTag P>
class Heuristic : public HeuristicBase<Heuristic<T, P>, P> { };




}  // namespace mimir

#endif  // MIMIR_SEARCH_HEURISTIC_TEMPLATE_HPP_

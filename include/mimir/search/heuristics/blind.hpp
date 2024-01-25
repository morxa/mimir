#ifndef MIMIR_SEARCH_HEURISTICS_BLIND_HPP_
#define MIMIR_SEARCH_HEURISTICS_BLIND_HPP_

#include "template.hpp"


namespace mimir
{

/**
 * Derived ID class.
 * 
 * Define name and template parameters of your own implementation.
*/
struct BlindTag : public HeuristicBaseTag { };


/**
 * Wrapper dispatch class.
 * 
 * Define the required template arguments of your implementation.
*/
template<IsHeuristicTag H, IsPlanningModeTag P, IsStateTag S, IsActionTag A>
struct is_wrapped_heuristic_tag<WrappedHeuristicTag<H, P, S, A>> : std::true_type {};


/**
 * Spezialized implementation class.
*/
template<IsPlanningModeTag P, IsStateTag S, IsActionTag A>
class Heuristic<WrappedHeuristicTag<BlindTag, P, S, A>>
    : public HeuristicBase<Heuristic<WrappedHeuristicTag<BlindTag, P, S, A>>> {
private:
    using StateView = View<WrappedStateTag<S, P>>;

    double compute_heuristic_impl(const StateView& state) {
        return 0.;
    }

    // Give access to the private interface implementations.
    template<typename>
    friend class HeuristicBase;

public:
    Heuristic(Problem problem) : HeuristicBase<Heuristic<WrappedHeuristicTag<BlindTag, P, S, A>>>(problem) { }
};


/**
 * Type traits.
*/
template<IsPlanningModeTag P, IsStateTag S, IsActionTag A>
struct TypeTraits<Heuristic<WrappedHeuristicTag<BlindTag, P, S, A>>> {
    using PlanningModeTag = P;
    using StateTag = S;
    using ActionTag = A;
};



}  // namespace mimir

#endif  // MIMIR_SEARCH_HEURISTICS_BLIND_HPP_

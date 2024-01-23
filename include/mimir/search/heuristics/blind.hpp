#ifndef MIMIR_SEARCH_HEURISTICS_BLIND_HPP_
#define MIMIR_SEARCH_HEURISTICS_BLIND_HPP_

#include "../heuristic_template.hpp"


namespace mimir
{

/**
 * ID class to dispatch a specialized implementation
*/
struct BlindTag : public HeuristicBaseTag { };


/**
 * Spezialized implementation class.
*/
template<IsPlanningModeTag P>
class Heuristic<HeuristicInstantiation<BlindTag,P>, P> : public HeuristicBase<Heuristic<HeuristicInstantiation<BlindTag,P>, P>, P> {
private:
    double compute_heuristic_impl(const View<State<P>>& state) {
        return 0.;
    }

    friend class HeuristicBase<Heuristic<HeuristicInstantiation<BlindTag,P>, P>, P>;

public:
    Heuristic(Problem problem) : HeuristicBase<Heuristic<HeuristicInstantiation<BlindTag,P>, P>, P>(problem) { }
};


}  // namespace mimir

#endif  // MIMIR_SEARCH_HEURISTICS_ZERO_HPP_

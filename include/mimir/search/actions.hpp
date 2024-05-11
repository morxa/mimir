#ifndef MIMIR_ACTIONS_HPP_
#define MIMIR_ACTIONS_HPP_

#include <vector>

/**
 * Include all specializations here
 */

#if defined(STATE_REPR_DENSE)

#include "mimir/search/actions/dense.hpp"

namespace mimir
{
using GroundActionBuilder = DenseActionBuilder;
using GroundAction = DenseAction;
using GroundActionList = std::vector<GroundAction>;
}

#else
#error "State representation undefined."
#endif

#endif  // MIMIR_ACTIONS_HPP_

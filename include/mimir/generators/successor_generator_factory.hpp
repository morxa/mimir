#ifndef MIMIR_PLANNERS_SUCCESSOR_GENERATOR_FACTORY_HPP_
#define MIMIR_PLANNERS_SUCCESSOR_GENERATOR_FACTORY_HPP_

#include "../formalism/declarations.hpp"
#include "successor_generator.hpp"

#include <memory>

namespace mimir::planners
{
    enum SuccessorGeneratorType
    {
        AUTOMATIC,
        LIFTED,
        GROUNDED
    };

    SuccessorGenerator create_sucessor_generator(const mimir::formalism::Repository& repository, SuccessorGeneratorType type);
}  // namespace planners

#endif  // MIMIR_PLANNERS_SUCCESSOR_GENERATOR_FACTORY_HPP_

#include <mimir/search/planners.hpp>

#include <gtest/gtest.h>

#include <string>


namespace mimir::tests
{

TEST(MimirTests, SearchPlannersParallelTest) {
    const auto domain_file = std::string("domain.pddl");
    const auto problem_file = std::string("problem.pddl");
    auto planner = Planner<ParallelTag<AStarTag<GroundedTag, BlindTag>, BrFSTag<LiftedTag>>>(domain_file, problem_file);
}



}

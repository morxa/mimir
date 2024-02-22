#include <mimir/formalism/problem/declarations.hpp>

#include <mimir/formalism/parser.hpp>

#include <mimir/search/successor_state_generators.hpp>

#include <gtest/gtest.h>


namespace mimir::tests
{

TEST(MimirTests, SearchSSGBitsetTest) {
    // Instantiate lifted version
    const auto domain_file = fs::path(std::string(DATA_DIR) + "gripper/domain.pddl");
    const auto problem_file = fs::path(std::string(DATA_DIR) + "gripper/problem.pddl");
    PDDLParser parser(domain_file, problem_file);
    auto lifted_ssg = SSG<SSGDispatcher<LiftedTag, BitsetStateTag>>(parser.get_problem());
    auto problem = static_cast<Problem>(nullptr);
    const auto initial_state = lifted_ssg.get_or_create_initial_state(problem);
}

}
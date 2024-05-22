#include "mimir/formalism/parser.hpp"
#include "mimir/search/algorithms.hpp"
#include "mimir/search/applicable_action_generators.hpp"

#include <gtest/gtest.h>

namespace mimir::tests
{

TEST(MimirTests, SearchAAGsDenseLiftedTest)
{
    const auto domain_file = fs::path(std::string(DATA_DIR) + "miconic-fulladl/domain.pddl");
    const auto problem_file = fs::path(std::string(DATA_DIR) + "miconic-fulladl/test_problem.pddl");
    PDDLParser parser(domain_file, problem_file);
    auto aag_event_handler = std::make_shared<DefaultLiftedAAGEventHandler>();
    auto aag = std::make_shared<LiftedDenseAAG>(parser.get_problem(), parser.get_factories(), aag_event_handler);
    auto ssg = std::make_shared<DenseSSG>(aag);
    auto algorithm_event_handler = std::make_shared<DefaultAlgorithmEventHandler>();
    auto brfs = BrFsAlgorithm(aag, ssg, algorithm_event_handler);
    auto ground_actions = GroundActionList {};
    const auto status = brfs.find_solution(ground_actions);
    EXPECT_EQ(status, SearchStatus::SOLVED);

    const auto& aag_statistics = aag_event_handler->get_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 84);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 10);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 377);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 16);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = algorithm_event_handler->get_statistics();

    EXPECT_FALSE(brfs_statistics.get_num_expanded_until_f_value().empty());
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 36);
}

}
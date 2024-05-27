#include "mimir/formalism/formalism.hpp"
#include "mimir/search/algorithms.hpp"
#include "mimir/search/algorithms/event_handlers.hpp"
#include "mimir/search/applicable_action_generators/dense_grounded/event_handlers.hpp"
#include "mimir/search/applicable_action_generators/dense_lifted/event_handlers.hpp"
#include "mimir/search/plan.hpp"

#include <gtest/gtest.h>

namespace mimir::tests
{

/// @brief Instantiate a lifted BrFs
class LiftedBrFsPlanner
{
private:
    PDDLParser m_parser;

    std::shared_ptr<ILiftedAAGEventHandler> m_aag_event_handler;
    std::shared_ptr<LiftedAAG> m_aag;
    std::shared_ptr<IDynamicSSG> m_ssg;
    std::shared_ptr<IAlgorithmEventHandler> m_algorithm_event_handler;
    std::unique_ptr<IAlgorithm> m_algorithm;

public:
    LiftedBrFsPlanner(const fs::path& domain_file, const fs::path& problem_file) :
        m_parser(PDDLParser(domain_file, problem_file)),
        m_aag_event_handler(std::make_shared<DefaultLiftedAAGEventHandler>()),
        m_aag(std::make_shared<LiftedAAG>(m_parser.get_problem(), m_parser.get_factories(), m_aag_event_handler)),
        m_ssg(std::make_shared<SuccessorStateGenerator>(m_aag)),
        m_algorithm_event_handler(std::make_shared<DefaultAlgorithmEventHandler>()),
        m_algorithm(std::make_unique<BrFsAlgorithm>(m_aag, m_ssg, m_algorithm_event_handler))
    {
    }

    std::tuple<SearchStatus, Plan> find_solution()
    {
        auto action_view_list = GroundActionList {};
        const auto status = m_algorithm->find_solution(action_view_list);
        return std::make_tuple(status, to_plan(action_view_list));
    }

    const AlgorithmStatistics& get_algorithm_statistics() const { return m_algorithm_event_handler->get_statistics(); }

    const LiftedAAGStatistics& get_aag_statistics() const { return m_aag_event_handler->get_statistics(); }
};

/// @brief Instantiate a grounded BrFs
class GroundedBrFsPlanner
{
private:
    PDDLParser m_parser;

    std::shared_ptr<IGroundedAAGEventHandler> m_aag_event_handler;
    std::shared_ptr<GroundedAAG> m_aag;
    std::shared_ptr<IDynamicSSG> m_ssg;
    std::shared_ptr<IAlgorithmEventHandler> m_algorithm_event_handler;
    std::unique_ptr<IAlgorithm> m_algorithm;

public:
    GroundedBrFsPlanner(const fs::path& domain_file, const fs::path& problem_file) :
        m_parser(PDDLParser(domain_file, problem_file)),
        m_aag_event_handler(std::make_shared<DefaultGroundedAAGEventHandler>()),
        m_aag(std::make_shared<GroundedAAG>(m_parser.get_problem(), m_parser.get_factories(), m_aag_event_handler)),
        m_ssg(std::make_shared<SuccessorStateGenerator>(m_aag)),
        m_algorithm_event_handler(std::make_shared<DefaultAlgorithmEventHandler>()),
        m_algorithm(std::make_unique<BrFsAlgorithm>(m_aag, m_ssg, m_algorithm_event_handler))
    {
    }

    std::tuple<SearchStatus, Plan> find_solution()
    {
        auto action_view_list = GroundActionList {};
        const auto status = m_algorithm->find_solution(action_view_list);
        return std::make_tuple(status, to_plan(action_view_list));
    }

    const AlgorithmStatistics& get_algorithm_statistics() const { return m_algorithm_event_handler->get_statistics(); }

    const GroundedAAGStatistics& get_aag_statistics() const { return m_aag_event_handler->get_statistics(); }
};

/// @brief Instantiate a BrFs
class BrFsPlanner
{
private:
    PDDLParser m_parser;

    std::unique_ptr<IAlgorithm> m_algorithm;

public:
    BrFsPlanner(const fs::path& domain_file, const fs::path& problem_file, bool grounded) :
        m_parser(PDDLParser(domain_file, problem_file)),
        m_algorithm(nullptr)
    {
        auto successor_generator = (grounded) ?
                                       std::shared_ptr<IDynamicAAG> { std::make_shared<GroundedDenseAAG>(m_parser.get_problem(), m_parser.get_factories()) } :
                                       std::shared_ptr<IDynamicAAG> { std::make_shared<LiftedDenseAAG>(m_parser.get_problem(), m_parser.get_factories()) };

        m_algorithm = std::make_unique<BrFsAlgorithm>(successor_generator);
    }

    std::tuple<SearchStatus, Plan> find_solution()
    {
        auto action_view_list = GroundActionList {};
        const auto status = m_algorithm->find_solution(action_view_list);
        return std::make_tuple(status, to_plan(action_view_list));
    }
};

/**
 * agricola-opt18-strips
 *
 * IPC instances are too difficult
 */

/**
 * Airport
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedAirportTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "airport/domain.pddl"), fs::path(std::string(DATA_DIR) + "airport/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 8);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 58);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 778);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 43);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 1378);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 43);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 138);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 840);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 26);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 20);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 18);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedAirportTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "airport/domain.pddl"), fs::path(std::string(DATA_DIR) + "airport/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 8);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 20);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 868);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 980);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 504);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 20);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 18);
}

/**
 * Barman
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedBarmanTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "barman/domain.pddl"), fs::path(std::string(DATA_DIR) + "barman/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 11);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 26);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 84);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 84);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 249);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 708);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 230);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedBarmanTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "barman/domain.pddl"), fs::path(std::string(DATA_DIR) + "barman/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 11);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 668);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 40);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 708);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 230);
}

/**
 * Blocks 3 ops
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedBlocks3opsTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "blocks_3/domain.pddl"), fs::path(std::string(DATA_DIR) + "blocks_3/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 15);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 45);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 45);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 96);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 68);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 21);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedBlocks3opsTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "blocks_3/domain.pddl"), fs::path(std::string(DATA_DIR) + "blocks_3/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 41);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 27);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 68);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 21);
}

/**
 * Blocks 4 ops
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedBlocks4opsTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "blocks_4/domain.pddl"), fs::path(std::string(DATA_DIR) + "blocks_4/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 19);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 24);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 24);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 72);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 21);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 9);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedBlocks4opsTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "blocks_4/domain.pddl"), fs::path(std::string(DATA_DIR) + "blocks_4/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 6);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 15);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 21);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 9);
}

/**
 * Childsnack
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedChildsnackTest)
{
    auto brfs =
        GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "childsnack/domain.pddl"), fs::path(std::string(DATA_DIR) + "childsnack/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 8);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 7);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 7);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 12);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 16);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 6);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedChildsnackTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "childsnack/domain.pddl"), fs::path(std::string(DATA_DIR) + "childsnack/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 9);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 7);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 16);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 6);
}

/**
 * Delivery
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedDeliveryTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "delivery/domain.pddl"), fs::path(std::string(DATA_DIR) + "delivery/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 10);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 16);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 16);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 32);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 18);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 7);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedDeliveryTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "delivery/domain.pddl"), fs::path(std::string(DATA_DIR) + "delivery/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 6);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 12);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 18);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 7);
}

/**
 * Driverlog
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedDriverlogTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "driverlog/domain.pddl"), fs::path(std::string(DATA_DIR) + "driverlog/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 9);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 10);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 14);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 14);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 28);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 57);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 23);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedDriverlogTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "driverlog/domain.pddl"), fs::path(std::string(DATA_DIR) + "driverlog/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 9);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 43);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 14);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 57);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 23);
}

/**
 * Ferry
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedFerryTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "ferry/domain.pddl"), fs::path(std::string(DATA_DIR) + "ferry/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 7);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 9);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 12);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 12);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 31);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 28);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 14);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedFerryTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "ferry/domain.pddl"), fs::path(std::string(DATA_DIR) + "ferry/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 7);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 18);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 10);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 28);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 14);
}

/**
 * Grid
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedGridTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "grid/domain.pddl"), fs::path(std::string(DATA_DIR) + "grid/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 21);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 35);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 35);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 82);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 18);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 7);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedGridTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "grid/domain.pddl"), fs::path(std::string(DATA_DIR) + "grid/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 5);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 13);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 18);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 7);
}

/**
 * Gripper
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedGripperTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "gripper/domain.pddl"), fs::path(std::string(DATA_DIR) + "gripper/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 3);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 12);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 20);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 20);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 48);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 44);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 12);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedGripperTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "gripper/domain.pddl"), fs::path(std::string(DATA_DIR) + "gripper/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 3);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 28);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 16);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 44);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 12);
}

/**
 * Hiking
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedHikingTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "hiking/domain.pddl"), fs::path(std::string(DATA_DIR) + "hiking/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 12);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 57);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 41);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 60);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 145);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 24);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedHikingTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "hiking/domain.pddl"), fs::path(std::string(DATA_DIR) + "hiking/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 104);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 41);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 145);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 24);
}

/**
 * Logistics
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedLogisticsTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "logistics/domain.pddl"), fs::path(std::string(DATA_DIR) + "logistics/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 9);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 14);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 14);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 26);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 43);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 8);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedLogisticsTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "logistics/domain.pddl"), fs::path(std::string(DATA_DIR) + "logistics/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 30);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 13);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 43);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 8);
}

/**
 * Miconic
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedMiconicTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "miconic/domain.pddl"), fs::path(std::string(DATA_DIR) + "miconic/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 5);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 8);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 6);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 6);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 14);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 26);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 14);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedMiconicTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "miconic/domain.pddl"), fs::path(std::string(DATA_DIR) + "miconic/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 5);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 20);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 6);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 26);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 14);
}

/**
 * Miconic-fulladl
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedMiconicFullAdlTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "miconic-fulladl/domain.pddl"),
                                    fs::path(std::string(DATA_DIR) + "miconic-fulladl/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 7);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 9);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 8);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 7);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 20);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 10);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 12);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 16);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 12);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 94);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 36);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedMiconicFullAdlTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "miconic-fulladl/domain.pddl"),
                                  fs::path(std::string(DATA_DIR) + "miconic-fulladl/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 7);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 84);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 10);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 377);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 16);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 94);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 36);
}

/**
 * Miconic-simpleadl
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedMiconicSimpleAdlTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "miconic-simpleadl/domain.pddl"),
                                    fs::path(std::string(DATA_DIR) + "miconic-simpleadl/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 4);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 4);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 4);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 4);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 8);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 4);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedMiconicSimpleAdlTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "miconic-simpleadl/domain.pddl"),
                                  fs::path(std::string(DATA_DIR) + "miconic-simpleadl/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 4);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 4);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 8);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 4);
}

/**
 * Reward
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedRewardTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "reward/domain.pddl"), fs::path(std::string(DATA_DIR) + "reward/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 7);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 6);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 6);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 10);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 12);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 7);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedRewardTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "reward/domain.pddl"), fs::path(std::string(DATA_DIR) + "reward/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 6);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 6);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 12);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 7);
}

/**
 * Rovers
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedRoversTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "rovers/domain.pddl"), fs::path(std::string(DATA_DIR) + "rovers/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 12);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 7);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 7);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 19);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 24);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 10);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedRoversTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "rovers/domain.pddl"), fs::path(std::string(DATA_DIR) + "rovers/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 17);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 7);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 24);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 10);
}

/**
 * Satellite
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedSatelliteTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "satellite/domain.pddl"), fs::path(std::string(DATA_DIR) + "satellite/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 7);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 12);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 18);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 18);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 21);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 303);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 56);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedSatelliteTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "satellite/domain.pddl"), fs::path(std::string(DATA_DIR) + "satellite/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 7);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 285);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 18);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 303);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 56);
}

/**
 * Schedule
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedScheduleTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "schedule/domain.pddl"), fs::path(std::string(DATA_DIR) + "schedule/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 2);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 45);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 49);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 49);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 144);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 884);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 45);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedScheduleTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "schedule/domain.pddl"), fs::path(std::string(DATA_DIR) + "schedule/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 2);

    const auto& aag_statistics = brfs.get_aag_statistics();

    // TODO: These asserts are disabled until the values are stable.
    // EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 907);
    // EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 49);
    // EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 72);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 884);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 45);
}

/**
 * Spanner
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedSpannerTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "spanner/domain.pddl"), fs::path(std::string(DATA_DIR) + "spanner/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 9);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 4);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 4);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 12);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 5);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 5);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedSpannerTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "spanner/domain.pddl"), fs::path(std::string(DATA_DIR) + "spanner/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 4);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 1);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 4);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 5);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 5);
}

/**
 * Transport
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedTransportTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "transport/domain.pddl"), fs::path(std::string(DATA_DIR) + "transport/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 5);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 26);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 104);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 104);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 246);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 384);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 85);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedTransportTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "transport/domain.pddl"), fs::path(std::string(DATA_DIR) + "transport/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 5);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 332);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 52);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 384);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 85);
}

/**
 * Visitall
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedVisitallTest)
{
    auto brfs = GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "visitall/domain.pddl"), fs::path(std::string(DATA_DIR) + "visitall/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 8);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 14);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 12);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 12);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 14);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 77);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 41);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedVisitallTest)
{
    auto brfs = LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "visitall/domain.pddl"), fs::path(std::string(DATA_DIR) + "visitall/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 8);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 65);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 12);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 77);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 41);
}

/**
 * Woodworking
 */

TEST(MimirTests, SearchAlgorithmsBrFSGroundedWoodworkingTest)
{
    auto brfs =
        GroundedBrFsPlanner(fs::path(std::string(DATA_DIR) + "woodworking/domain.pddl"), fs::path(std::string(DATA_DIR) + "woodworking/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 2);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_fluent_ground_atoms(), 19);
    EXPECT_EQ(aag_statistics.get_num_delete_free_reachable_derived_ground_atoms(), 0);
    EXPECT_EQ(aag_statistics.get_num_delete_free_actions(), 57);
    EXPECT_EQ(aag_statistics.get_num_delete_free_axioms(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_actions(), 57);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_action_match_tree(), 110);

    EXPECT_EQ(aag_statistics.get_num_ground_axioms(), 0);
    EXPECT_EQ(aag_statistics.get_num_nodes_in_axiom_match_tree(), 1);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 10);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 3);
}

TEST(MimirTests, SearchAlgorithmsBrFSLiftedWoodworkingTest)
{
    auto brfs =
        LiftedBrFsPlanner(fs::path(std::string(DATA_DIR) + "woodworking/domain.pddl"), fs::path(std::string(DATA_DIR) + "woodworking/test_problem.pddl"));
    const auto [search_status, plan] = brfs.find_solution();
    EXPECT_EQ(search_status, SearchStatus::SOLVED);
    EXPECT_EQ(plan.get_actions().size(), 2);

    const auto& aag_statistics = brfs.get_aag_statistics();

    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_hits_until_f_value().back(), 1);
    EXPECT_EQ(aag_statistics.get_num_ground_action_cache_misses_until_f_value().back(), 9);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_actions_until_f_value().back(), 0);

    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_hits_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_ground_axiom_cache_misses_until_f_value().back(), 0);
    EXPECT_EQ(aag_statistics.get_num_inapplicable_grounded_axioms_until_f_value().back(), 0);

    const auto& brfs_statistics = brfs.get_algorithm_statistics();

    EXPECT_EQ(brfs_statistics.get_num_generated_until_f_value().back(), 10);
    EXPECT_EQ(brfs_statistics.get_num_expanded_until_f_value().back(), 3);
}

}

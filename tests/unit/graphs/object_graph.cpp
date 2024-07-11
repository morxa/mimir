#include "mimir/graphs/object_graph.hpp"

#include "mimir/algorithms/nauty.hpp"
#include "mimir/datasets/state_space.hpp"
#include "mimir/graphs/certificate.hpp"

#include <gtest/gtest.h>
#include <loki/loki.hpp>
#include <unordered_set>

namespace mimir::tests
{

TEST(MimirTests, GraphsObjectGraphDenseTest)
{
    const auto domain_file = fs::path(std::string(DATA_DIR) + "gripper/domain.pddl");
    const auto problem_file = fs::path(std::string(DATA_DIR) + "gripper/p-2-0.pddl");

    const auto state_space = StateSpace::create(domain_file, problem_file).value();

    const auto color_function = ProblemColorFunction(state_space.get_problem());
    auto certificates = std::unordered_set<Certificate, loki::Hash<Certificate>, loki::EqualTo<Certificate>> {};

    for (const auto& state : state_space.get_states())
    {
        // std::cout << std::make_tuple(state_space->get_aag()->get_problem(), state, std::cref(state_space->get_aag()->get_pddl_factories())) << std::endl;

        const auto object_graph = create_object_graph(color_function, *state_space.get_pddl_factories(), state_space.get_problem(), state);

        // std::cout << object_graph << std::endl;

        auto certificate = Certificate(nauty_wrapper::DenseGraph(object_graph).compute_certificate(), compute_sorted_vertex_colors(object_graph));

        certificates.insert(std::move(certificate));
    }

    EXPECT_EQ(state_space.get_states().size(), 28);
    EXPECT_EQ(certificates.size(), 12);
}

TEST(MimirTests, GraphsObjectGraphSparseTest)
{
    const auto domain_file = fs::path(std::string(DATA_DIR) + "gripper/domain.pddl");
    const auto problem_file = fs::path(std::string(DATA_DIR) + "gripper/p-2-0.pddl");

    const auto state_space = StateSpace::create(domain_file, problem_file).value();

    const auto color_function = ProblemColorFunction(state_space.get_problem());
    auto certificates = std::unordered_set<Certificate, loki::Hash<Certificate>, loki::EqualTo<Certificate>> {};

    for (const auto& state : state_space.get_states())
    {
        // std::cout << std::make_tuple(state_space->get_aag()->get_problem(), state, std::cref(state_space->get_aag()->get_pddl_factories())) << std::endl;

        const auto object_graph = create_object_graph(color_function, *state_space.get_pddl_factories(), state_space.get_problem(), state);

        // std::cout << object_graph << std::endl;

        auto certificate = Certificate(nauty_wrapper::SparseGraph(object_graph).compute_certificate(), compute_sorted_vertex_colors(object_graph));

        certificates.insert(std::move(certificate));
    }

    EXPECT_EQ(state_space.get_states().size(), 28);
    EXPECT_EQ(certificates.size(), 12);
}

TEST(MimirTests, GraphsObjectGraphPruningTest)
{
    const auto domain_file = fs::path(std::string(DATA_DIR) + "gripper/domain.pddl");
    const auto problem_file = fs::path(std::string(DATA_DIR) + "gripper/p-2-0.pddl");
    const auto state_space = StateSpace::create(domain_file, problem_file).value();

    class PruneAllObjects : public ObjectGraphPruningStrategy
    {
    public:
        bool prune(const Object&) const override { return true; };
        bool prune(const GroundAtom<Static>) const override { return true; };
        bool prune(const GroundAtom<Fluent>) const override { return true; };
        bool prune(const GroundAtom<Derived>) const override { return true; };
        bool prune(const GroundLiteral<Static>) const override { return true; }
        bool prune(const GroundLiteral<Fluent>) const override { return true; }
        bool prune(const GroundLiteral<Derived>) const override { return true; }
    };

    const auto color_function = ProblemColorFunction(state_space.get_problem());
    auto certificates = std::unordered_set<Certificate, loki::Hash<Certificate>, loki::EqualTo<Certificate>> {};

    for (const auto& state : state_space.get_states())
    {
        // std::cout << std::make_tuple(state_space->get_aag()->get_problem(), state, std::cref(state_space->get_aag()->get_pddl_factories())) << std::endl;

        const auto object_graph =
            create_object_graph(color_function, *state_space.get_pddl_factories(), state_space.get_problem(), state, true, PruneAllObjects());

        // std::cout << object_graph << std::endl;

        auto certificate = Certificate(nauty_wrapper::SparseGraph(object_graph).compute_certificate(), compute_sorted_vertex_colors(object_graph));

        certificates.insert(std::move(certificate));
    }

    EXPECT_EQ(state_space.get_states().size(), 28);
    EXPECT_EQ(certificates.size(), 1);
}

}

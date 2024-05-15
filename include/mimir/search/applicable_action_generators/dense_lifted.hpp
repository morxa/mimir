#ifndef MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_DENSE_LIFTED_HPP_
#define MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_DENSE_LIFTED_HPP_

#include "mimir/formalism/declarations.hpp"
#include "mimir/formalism/factories.hpp"
#include "mimir/search/actions/dense.hpp"
#include "mimir/search/applicable_action_generators/dense_lifted/assignment_set.hpp"
#include "mimir/search/applicable_action_generators/dense_lifted/consistency_graph.hpp"
#include "mimir/search/applicable_action_generators/dense_lifted/grounding_table.hpp"
#include "mimir/search/applicable_action_generators/interface.hpp"
#include "mimir/search/axiom_evaluators/dense.hpp"
#include "mimir/search/axioms/dense.hpp"
#include "mimir/search/states/dense.hpp"

#include <flatmemory/details/view_const.hpp>
#include <unordered_map>
#include <vector>

namespace mimir
{

using GroundFunctionToValue = std::unordered_map<GroundFunction, double>;

/**
 * Fully specialized implementation class.
 *
 * Implements successor generation using maximum clique enumeration by stahlberg-ecai2023
 * Source: https://mrlab.ai/papers/stahlberg-ecai2023.pdf
 */
template<>
class AAG<LiftedAAGDispatcher<DenseStateTag>> : public IStaticAAG<AAG<LiftedAAGDispatcher<DenseStateTag>>>
{
private:
    Problem m_problem;
    PDDLFactories& m_pddl_factories;

    DenseAE m_axiom_evaluator;

    DenseGroundActionSet m_applicable_actions;
    FlatDenseActionSet m_actions;
    DenseGroundActionList m_actions_by_index;
    DenseGroundActionBuilder m_action_builder;
    std::unordered_map<Action, GroundingTable<DenseGroundAction>> m_action_groundings;

    GroundFunctionToValue m_ground_function_value_costs;

    std::unordered_map<Action, consistency_graph::Graphs> m_static_consistency_graphs;

    /// @brief Returns true if all nullary literals in the precondition hold, false otherwise.
    bool nullary_preconditions_hold(const Action& action, DenseState state) const;

    void nullary_case(const Action& action, DenseState state, DenseGroundActionList& out_applicable_actions);

    void unary_case(const Action& action, DenseState state, DenseGroundActionList& out_applicable_actions);

    void general_case(const AssignmentSet& assignment_sets, const Action& action, DenseState state, DenseGroundActionList& out_applicable_actions);

    /// @brief Ground the precondition of an action and return a view onto it.
    [[nodiscard]] DenseGroundAction ground_action_precondition(const Action& action, const ObjectList& binding);

    /* Implement IStaticAAG interface */
    friend class IStaticAAG<AAG<LiftedAAGDispatcher<DenseStateTag>>>;

    void generate_applicable_actions_impl(const DenseState state, DenseGroundActionList& out_applicable_actions);

    void generate_and_apply_axioms_impl(FlatBitsetBuilder& ref_state_atoms);

public:
    AAG(Problem problem, PDDLFactories& pddl_factories);

    /// @brief Return the axiom partitioning.
    [[nodiscard]] const std::vector<AxiomPartition>& get_axiom_partitioning() const;

    /// @brief Ground an axiom and return a view onto it.
    [[nodiscard]] GroundAxiom ground_axiom(const Axiom& axiom, ObjectList&& binding);

    /// @brief Ground an action and return a view onto it.
    [[nodiscard]] DenseGroundAction ground_action(const Action& action, ObjectList&& binding);

    /// @brief Return all applicable axioms.
    [[nodiscard]] const DenseGroundAxiomSet& get_applicable_axioms() const;

    /// @brief Return all axioms.
    [[nodiscard]] const FlatDenseAxiomSet& get_axioms() const;

    /// @brief Return all applicable actions.
    [[nodiscard]] const DenseGroundActionSet& get_applicable_actions() const;

    /// @brief Return all actions.
    [[nodiscard]] const FlatDenseActionSet& get_actions() const;

    /// @brief Return the action with the given id.
    [[nodiscard]] DenseGroundAction get_action(size_t action_id) const;
};

/**
 * Types
 */

using LiftedDenseAAG = AAG<LiftedAAGDispatcher<DenseStateTag>>;

}  // namespace mimir

#endif
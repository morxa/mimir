/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "mimir/search/applicable_action_generators/dense_grounded.hpp"

#include "mimir/common/collections.hpp"
#include "mimir/common/itertools.hpp"
#include "mimir/common/printers.hpp"
#include "mimir/formalism/transformers/delete_relax.hpp"
#include "mimir/search/algorithms/brfs.hpp"
#include "mimir/search/applicable_action_generators/dense_lifted.hpp"
#include "mimir/search/applicable_action_generators/interface.hpp"
#include "mimir/search/axiom_evaluators/axiom_stratification.hpp"
#include "mimir/search/event_handlers/minimal.hpp"
#include "mimir/search/successor_state_generators/dense.hpp"

#include <deque>

namespace mimir
{

AAG<GroundedAAGDispatcher<DenseStateTag>>::AAG(Problem problem, PDDLFactories& pddl_factories) :
    m_problem(problem),
    m_pddl_factories(pddl_factories),
    m_lifted_aag(m_problem, m_pddl_factories)
{
    // 1. Explore delete relaxed task. We explicitly require to keep actions and axioms with empty effects.
    auto delete_relax_transformer = DeleteRelaxTransformer(m_pddl_factories, false);
    const auto dr_problem = delete_relax_transformer.run(*m_problem);
    auto dr_lifted_aag = std::make_shared<LiftedDenseAAG>(dr_problem, m_pddl_factories);
    auto dr_ssg = DenseSSG(dr_problem, dr_lifted_aag);

    // TODO provide conversion from view to builder in flatmemory
    auto& state_bitset = m_state_builder.get_atoms_bitset();
    state_bitset.unset_all();
    for (const auto& atom_id : dr_ssg.get_or_create_initial_state(m_problem).get_atoms_bitset())
    {
        state_bitset.set(atom_id);
    }

    std::cout << *dr_problem->get_domain() << std::endl;
    std::cout << *dr_problem << std::endl;

    // Keep track of changes
    size_t num_atoms = 0;
    size_t num_actions = 0;
    // Temporary variables
    auto state_atoms = FlatBitsetBuilder();
    auto derived_atoms = FlatBitsetBuilder();
    auto actions = DenseActionList {};
    do
    {
        num_atoms = m_pddl_factories.get_ground_atoms().size();
        num_actions = dr_lifted_aag->get_actions().size();

        m_state_builder.get_flatmemory_builder().finish();
        const auto state = DenseState(FlatDenseState(m_state_builder.get_flatmemory_builder().buffer().data()));

        // Create and all applicable actions and apply them
        dr_lifted_aag->generate_applicable_actions(state, actions);
        for (const auto& action : actions)
        {
            const auto is_newly_generated = (action.get_id() >= num_actions);
            if (is_newly_generated)
            {
                const auto succ_state = dr_ssg.get_or_create_successor_state(state, action);
                for (const auto atom_id : succ_state.get_atoms_bitset())
                {
                    state_bitset.set(atom_id);
                }
            }
        }

        // Create and all applicable axioms and apply them
        state_atoms |= state_bitset;
        dr_lifted_aag->generate_and_apply_axioms(state_atoms, derived_atoms);

    } while (num_atoms != m_pddl_factories.get_ground_atoms().size());

    std::cout << "Total number of ground atoms reachable in delete-relaxed task: " << m_pddl_factories.get_ground_atoms().size() << std::endl;
    std::cout << "Total number of ground actions in delete-relaxed task: " << dr_lifted_aag->get_actions().size() << std::endl;

    // 2. Create ground actions
    auto ground_actions = DenseActionList {};
    for (const auto& action : dr_lifted_aag->get_actions())
    {
        // Map relaxed to unrelaxed actions and ground them with the same arguments.
        const auto action_proxy = DenseAction(action);
        for (const auto& unrelaxed_action : delete_relax_transformer.get_unrelaxed_actions(action_proxy.get_action()))
        {
            auto action_arguments = ObjectList(action_proxy.get_objects().begin(), action_proxy.get_objects().end());
            ground_actions.push_back(m_lifted_aag.ground_action(unrelaxed_action, std::move(action_arguments)));
        }
    }

    // 3. Build match tree
    m_action_match_tree = MatchTree(m_pddl_factories.get_ground_atoms().size(), ground_actions);

    // 2. Create ground axioms
    auto ground_axioms = DenseAxiomList {};

    // ERROR here: some ground axioms are not being generated
    /*
    for (const auto& axiom : dr_lifted_aag->get_axioms())
    {
        // Map relaxed to unrelaxed actions and ground them with the same arguments.
        const auto axiom_proxy = DenseAxiom(axiom);
        for (const auto& unrelaxed_axiom : delete_relax_transformer.get_unrelaxed_axioms(axiom_proxy.get_axiom()))
        {
            auto axiom_arguments = ObjectList(axiom_proxy.get_objects().begin(), axiom_proxy.get_objects().end());
            ground_axioms.push_back(m_lifted_aag.ground_axiom(unrelaxed_axiom, std::move(axiom_arguments)));
        }
    }
    */

    // Current solution that works: exhaustively generate ground axioms for testing
    auto axioms = m_problem->get_axioms();
    auto add_axioms = m_problem->get_domain()->get_axioms();
    axioms.insert(axioms.end(), add_axioms.begin(), add_axioms.end());

    for (const auto& axiom : axioms)
    {
        if (axiom->get_arity() == 0)
        {
            ground_axioms.push_back(m_lifted_aag.ground_axiom(axiom, ObjectList {}));
        }
        else
        {
            auto vecs = std::vector<ObjectList>(axiom->get_arity(), m_problem->get_objects());
            for (const auto& combination_it : Combinations(vecs))
            {
                auto binding = ObjectList {};
                for (const auto& object_it : combination_it)
                {
                    binding.push_back(*object_it);
                }
                ground_axioms.push_back(m_lifted_aag.ground_axiom(axiom, std::move(binding)));
            }
        }
    }

    // 3. Build match tree
    m_axiom_match_tree = MatchTree(m_pddl_factories.get_ground_atoms().size(), ground_axioms);

    std::cout << "Total number of ground actions in task: " << ground_actions.size() << std::endl;
    std::cout << "Total number of ground axioms in task: " << ground_axioms.size() << std::endl;
    std::cout << "Total number of nodes in action match tree: " << m_action_match_tree.get_num_nodes() << std::endl;
    std::cout << "Total number of nodes in axiom match tree: " << m_axiom_match_tree.get_num_nodes() << std::endl;
}

void AAG<GroundedAAGDispatcher<DenseStateTag>>::generate_applicable_actions_impl(DenseState state, DenseActionList& out_applicable_actions)
{
    out_applicable_actions.clear();

    m_action_match_tree.get_applicable_elements(state.get_atoms_bitset(), out_applicable_actions);
}

void AAG<GroundedAAGDispatcher<DenseStateTag>>::generate_and_apply_axioms_impl(FlatBitsetBuilder& ref_state_atoms, FlatBitsetBuilder& ref_derived_atoms)
{
    // Lifted works so there must be an issue with generating applicable axioms and applying them in grounded setting.
    // m_lifted_aag.generate_and_apply_axioms(ref_state_atoms, ref_derived_atoms);
    // return;

    for (const auto& lifted_partition : m_lifted_aag.get_axiom_partitioning())
    {
        bool reached_partition_fixed_point;

        do
        {
            reached_partition_fixed_point = true;

            /* Compute applicable axioms. */

            auto applicable_axioms = GroundAxiomList {};
            auto tmp_builder = FlatBitsetBuilder();
            tmp_builder.unset_all();
            tmp_builder |= ref_state_atoms;
            tmp_builder.finish();
            auto state_bitset = FlatBitset(tmp_builder.buffer().data());
            m_axiom_match_tree.get_applicable_elements(state_bitset, applicable_axioms);

            /* Apply applicable axioms */

            for (const auto& grounded_axiom : applicable_axioms)
            {
                assert(grounded_axiom.get_axiom());
                if (!lifted_partition.get_axioms().count(grounded_axiom.get_axiom()))
                {
                    // axiom not part of same partition
                    continue;
                }

                assert(grounded_axiom.is_applicable(ref_state_atoms));

                assert(!grounded_axiom.get_simple_effect().is_negated);

                const auto grounded_atom_id = grounded_axiom.get_simple_effect().atom_id;

                if (!ref_state_atoms.get(grounded_atom_id))
                {
                    // GENERATED NEW DERIVED ATOM!
                    reached_partition_fixed_point = false;
                }

                ref_state_atoms.set(grounded_atom_id);
                ref_derived_atoms.set(grounded_atom_id);
            }

        } while (!reached_partition_fixed_point);
    }
}

[[nodiscard]] const FlatDenseActionSet& AAG<GroundedAAGDispatcher<DenseStateTag>>::get_actions() const { return m_lifted_aag.get_actions(); }

[[nodiscard]] ConstView<ActionDispatcher<DenseStateTag>> AAG<GroundedAAGDispatcher<DenseStateTag>>::get_action(size_t action_id) const
{
    return m_lifted_aag.get_action(action_id);
}
}

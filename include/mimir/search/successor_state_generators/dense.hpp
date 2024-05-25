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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MIMIR_SEARCH_SUCCESSOR_STATE_GENERATORS_DENSE_HPP_
#define MIMIR_SEARCH_SUCCESSOR_STATE_GENERATORS_DENSE_HPP_

#include "mimir/common/collections.hpp"
#include "mimir/formalism/formalism.hpp"
#include "mimir/search/applicable_action_generators.hpp"
#include "mimir/search/applicable_action_generators/dense_lifted/assignment_set.hpp"
#include "mimir/search/states/dense.hpp"
#include "mimir/search/successor_state_generators/interface.hpp"

#include <cstddef>
#include <flatmemory/flatmemory.hpp>
#include <stdexcept>

namespace mimir
{

/**
 * Implementation class
 */
template<>
class SSG<SSGDispatcher<DenseStateTag>> : public IStaticSSG<SSG<SSGDispatcher<DenseStateTag>>>
{
private:
    std::shared_ptr<IDynamicAAG> m_aag;

    // TODO: Is it possible to separate ground atoms over simple and derived predicates
    // to store extended and non-extended states more efficiently?
    FlatDenseStateSet m_states;
    std::vector<DenseState> m_states_by_index;
    DenseStateBuilder m_state_builder;

    /* Implement IStaticSSG interface */
    friend class IStaticSSG<SSG<SSGDispatcher<DenseStateTag>>>;

    [[nodiscard]] DenseState get_or_create_initial_state_impl()
    {
        auto ground_atoms = GroundAtomList<Fluent> {};

        for (const auto& literal : m_aag->get_problem()->get_fluent_initial_literals())
        {
            if (literal->is_negated())
            {
                throw std::runtime_error("negative literals in the initial state are not supported");
            }

            ground_atoms.push_back(literal->get_atom());
        }

        return get_or_create_state(ground_atoms);
    }

    [[nodiscard]] State get_or_create_state_impl(const GroundAtomList<Fluent>& atoms)
    {
        /* Header */

        int next_state_id = m_states.size();

        /* Fetch member references. */

        auto& state_id = m_state_builder.get_id();
        auto& fluent_state_atoms = m_state_builder.get_fluent_atoms();
        auto& derived_state_atoms = m_state_builder.get_derived_atoms();
        fluent_state_atoms.unset_all();
        derived_state_atoms.unset_all();
        auto& problem = m_state_builder.get_problem();

        /* 1. Set state id */

        state_id = next_state_id;

        /* 2. Set problem */

        problem = m_aag->get_problem();

        /* 3. Construct non-extended state */

        for (const auto& atom : atoms)
        {
            fluent_state_atoms.set(atom->get_identifier());
        }

        auto& flatmemory_builder = m_state_builder.get_flatmemory_builder();

        /* 4. Retrieve cached extended state */

        // Test whether there exists an extended state for the given non extended state
        flatmemory_builder.finish();
        auto iter = m_states.find(FlatDenseState(flatmemory_builder.buffer().data()));
        if (iter != m_states.end())
        {
            return DenseState(*iter);
        }

        /* 5. Construct extended state by evaluating Axioms */

        m_aag->generate_and_apply_axioms(fluent_state_atoms, derived_state_atoms);

        /* 6. Cache extended state */

        flatmemory_builder.finish();
        auto [iter2, inserted] = m_states.insert(flatmemory_builder);
        m_states_by_index.push_back(DenseState(*iter2));

        /* 7. Return newly generated extended state */

        return DenseState(*iter2);
    }

    [[nodiscard]] DenseState get_or_create_successor_state_impl(const DenseState state, const DenseGroundAction action)
    {
        int next_state_id = m_states.size();

        // Fetch member references.

        auto& state_id = m_state_builder.get_id();
        auto& fluent_state_atoms = m_state_builder.get_fluent_atoms();
        auto& derived_state_atoms = m_state_builder.get_derived_atoms();
        fluent_state_atoms.unset_all();
        derived_state_atoms.unset_all();
        auto& problem = m_state_builder.get_problem();

        // TODO: add assignment operator to bitset to replace unset + operator|=
        const auto& unextended_state = m_states_by_index[state.get_id()];
        fluent_state_atoms |= unextended_state.get_fluent_atoms();

        /* 1. Set state id */

        state_id = next_state_id;

        /* 2. Set problem */

        problem = m_aag->get_problem();

        /* 3. Construct non-extended state */

        /* Simple effects*/
        auto strips_part_proxy = DenseStripsActionEffect(action.get_strips_effect());
        fluent_state_atoms -= strips_part_proxy.get_negative_effects();
        fluent_state_atoms |= strips_part_proxy.get_positive_effects();

        /* Conditional effects */
        for (const auto flat_conditional_effect : action.get_conditional_effects())
        {
            auto cond_effect_proxy = DenseConditionalEffect(flat_conditional_effect);

            if (cond_effect_proxy.is_applicable(state))
            {
                const auto& simple_effect = cond_effect_proxy.get_simple_effect();

                if (simple_effect.is_negated)
                {
                    fluent_state_atoms.unset(simple_effect.atom_id);
                }
                else
                {
                    fluent_state_atoms.set(simple_effect.atom_id);
                }
            }
        }

        auto& flatmemory_builder = m_state_builder.get_flatmemory_builder();

        /* 4. Retrieve cached extended state */

        // Test whether there exists an extended state for the given non extended state
        flatmemory_builder.finish();
        auto iter = m_states.find(FlatDenseState(flatmemory_builder.buffer().data()));
        if (iter != m_states.end())
        {
            return DenseState(*iter);
        }

        /* 5. Construct extended state by evaluating Axioms */

        m_aag->generate_and_apply_axioms(fluent_state_atoms, derived_state_atoms);

        /* 6. Cache extended state */

        flatmemory_builder.finish();
        auto [iter2, inserted] = m_states.insert(flatmemory_builder);
        m_states_by_index.push_back(DenseState(*iter2));

        /* 7. Return newly generated extended state */

        return DenseState(*iter2);
    }

    [[nodiscard]] DenseState get_non_extended_state_impl(const DenseState state)
    {  //
        return m_states_by_index[state.get_id()];
    }

    [[nodiscard]] size_t get_state_count_impl() const { return m_states.size(); }

public:
    explicit SSG(std::shared_ptr<IDynamicAAG> aag) : m_aag(std::move(aag)), m_states_by_index() {}
};

/**
 * Types
 */

using DenseSSG = SSG<SSGDispatcher<DenseStateTag>>;

}

#endif
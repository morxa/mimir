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

#ifndef MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_DENSE_LIFTED_EVENT_HANDLERS_DEFAULT_HPP_
#define MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_DENSE_LIFTED_EVENT_HANDLERS_DEFAULT_HPP_

#include "mimir/search/applicable_action_generators/dense_lifted/event_handlers/interface.hpp"

namespace mimir
{

class DefaultLiftedAAGEventHandler : public LiftedAAGEventHandlerBase<DefaultLiftedAAGEventHandler>
{
private:
    /* Implement LiftedAAGEventHandlerBase interface */
    friend class LiftedAAGEventHandlerBase<DefaultLiftedAAGEventHandler>;

    void on_start_generating_applicable_actions_impl() const;

    void on_ground_action_impl(const Action action, const ObjectList& binding) const;

    void on_ground_action_cache_hit_impl(const Action action, const ObjectList& binding) const;

    void on_ground_action_cache_miss_impl(const Action action, const ObjectList& binding) const;

    void on_ground_inapplicable_action_impl(const GroundAction action, const PDDLFactories& pddl_factories) const;

    void on_end_generating_applicable_actions_impl(const GroundActionList& ground_actions, const PDDLFactories& pddl_factories) const;

    void on_start_generating_applicable_axioms_impl() const;

    void on_ground_axiom_impl(const Axiom axiom, const ObjectList& binding) const;

    void on_ground_axiom_cache_hit_impl(const Axiom axiom, const ObjectList& binding) const;

    void on_ground_axiom_cache_miss_impl(const Axiom axiom, const ObjectList& binding) const;

    void on_ground_inapplicable_axiom_impl(const GroundAxiom axiom, const PDDLFactories& pddl_factories) const;

    void on_end_generating_applicable_axioms_impl(const GroundAxiomList& ground_axioms, const PDDLFactories& pddl_factories) const;

    void on_finish_f_layer_impl() const;

    void on_end_search_impl() const;
};

}

#endif
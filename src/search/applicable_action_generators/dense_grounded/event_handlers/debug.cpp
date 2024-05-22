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

#include "mimir/search/applicable_action_generators/dense_grounded/event_handlers/debug.hpp"

#include "mimir/search/applicable_action_generators/dense_grounded/match_tree.hpp"

#include <iostream>

namespace mimir
{

void DebugGroundedAAGEventHandler::on_finish_delete_free_exploration_impl(const GroundAtomList& reached_atoms,
                                                                          const GroundActionList& instantiated_actions,
                                                                          const GroundAxiomList& instantiated_axioms)
{
    std::cout << "[Grounded AAG] Number of grounded atoms reachable in delete-free problem: " << reached_atoms.size() << "\n"
              << "[Grounded AAG] Number of delete-free grounded actions: " << instantiated_actions.size() << "\n"
              << "[Grounded AAG] Number of delete-free grounded axioms: " << instantiated_axioms.size() << std::endl;
}

void DebugGroundedAAGEventHandler::on_finish_grounding_unrelaxed_actions_impl(const GroundActionList& unrelaxed_actions)
{
    std::cout << "[Grounded AAG] Number of grounded actions in problem: " << unrelaxed_actions.size() << std::endl;
}

void DebugGroundedAAGEventHandler::on_finish_build_action_match_tree_impl(const MatchTree<GroundAction>& action_match_tree)
{
    std::cout << "[Grounded AAG] Number of nodes in action match tree: " << action_match_tree.get_num_nodes() << std::endl;
}

void DebugGroundedAAGEventHandler::on_finish_grounding_unrelaxed_axioms_impl(const GroundAxiomList& unrelaxed_axioms)
{
    std::cout << "[Grounded AAG] Number of grounded axioms in problem: " << unrelaxed_axioms.size() << std::endl;
}

void DebugGroundedAAGEventHandler::on_finish_build_axiom_match_tree_impl(const MatchTree<GroundAxiom>& axiom_match_tree)
{
    std::cout << "[Grounded AAG] Number of nodes in axiom match tree: " << axiom_match_tree.get_num_nodes() << std::endl;
}

void DebugGroundedAAGEventHandler::on_finish_f_layer_impl() const
{  //
}

void DebugGroundedAAGEventHandler::on_end_search_impl() const {}

}
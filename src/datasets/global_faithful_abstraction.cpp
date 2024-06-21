/*
 * Copyright (C) 2023 Simon Stahlberg
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

#include "mimir/datasets/global_faithful_abstraction.hpp"

#include "mimir/algorithms/BS_thread_pool.hpp"
#include "mimir/common/timers.hpp"

#include <algorithm>
#include <cstdlib>
#include <deque>

namespace mimir
{
/**
 * GlobalFaithfulAbstractState
 */

GlobalFaithfulAbstractState::GlobalFaithfulAbstractState(StateId id, AbstractionId abstraction_id, StateId abstract_state_id) :
    m_id(id),
    m_abstraction_id(abstraction_id),
    m_abstract_state_id(abstract_state_id)
{
}

bool GlobalFaithfulAbstractState::operator==(const GlobalFaithfulAbstractState& other) const
{
    if (this != &other)
    {
        // same id implies same abstraction_id and abstract_state_id
        return (m_id == other.m_id);
    }
    return true;
}

size_t GlobalFaithfulAbstractState::hash() const
{
    // same id implies same abstraction_id and abstract_state_id
    return loki::hash_combine(m_id);
}

StateId GlobalFaithfulAbstractState::get_id() const { return m_id; }

AbstractionId GlobalFaithfulAbstractState::get_abstraction_id() const { return m_abstraction_id; }

StateId GlobalFaithfulAbstractState::get_abstract_state_id() const { return m_abstract_state_id; }

/**
 * GlobalFaithfulAbstraction
 */

GlobalFaithfulAbstraction::GlobalFaithfulAbstraction(bool mark_true_goal_atoms,
                                                     bool use_unit_cost_one,
                                                     AbstractionId id,
                                                     std::shared_ptr<FaithfulAbstractionList> abstractions,
                                                     GlobalFaithfulAbstractStateList states,
                                                     CertificateToStateIdMap states_by_certificate,
                                                     size_t num_isomorphic_states,
                                                     size_t num_non_isomorphic_states) :
    m_mark_true_goal_atoms(mark_true_goal_atoms),
    m_use_unit_cost_one(use_unit_cost_one),
    m_id(id),
    m_abstractions(std::move(abstractions)),
    m_states(std::move(states)),
    m_states_by_certificate(std::move(states_by_certificate)),
    m_num_isomorphic_states(num_isomorphic_states),
    m_num_non_isomorphic_states(num_non_isomorphic_states),
    m_nauty_graph(),
    m_object_graph_factory(m_abstractions->at(m_id).get_aag()->get_problem(), m_abstractions->at(m_id).get_pddl_factories(), m_mark_true_goal_atoms)
{
}

std::vector<GlobalFaithfulAbstraction> GlobalFaithfulAbstraction::create(const fs::path& domain_filepath,
                                                                         const std::vector<fs::path>& problem_filepaths,
                                                                         bool mark_true_goal_atoms,
                                                                         bool use_unit_cost_one,
                                                                         uint32_t max_num_states,
                                                                         uint32_t timeout_ms,
                                                                         uint32_t num_threads)
{
    auto abstractions = std::vector<GlobalFaithfulAbstraction> {};
    auto faithful_abstractions =
        FaithfulAbstraction::create(domain_filepath, problem_filepaths, mark_true_goal_atoms, use_unit_cost_one, max_num_states, timeout_ms, num_threads);

    auto certificate_to_global_state = std::unordered_map<Certificate, GlobalFaithfulAbstractState, loki::Hash<Certificate>, loki::EqualTo<Certificate>> {};

    // An abstraction is considered relevant, if it contains at least one non-isomorphic state.
    auto relevant_faithful_abstractions = std::make_shared<FaithfulAbstractionList>();
    auto abstraction_id = AbstractionId { 0 };

    for (auto& faithful_abstraction : faithful_abstractions)
    {
        auto max_goal_distance = *std::max_element(faithful_abstraction.get_goal_distances().begin(), faithful_abstraction.get_goal_distances().end());

        auto is_relevant = false;
        for (size_t state_id = 0; state_id < faithful_abstraction.get_num_states(); ++state_id)
        {
            const auto& state = faithful_abstraction.get_states().at(state_id);
            if (faithful_abstraction.get_goal_distances().at(state_id) == max_goal_distance  //
                && !certificate_to_global_state.count(state.get_certificate()))
            {
                is_relevant = true;
                break;
            }
        }

        if (!is_relevant)
        {
            continue;
        }

        auto num_isomorphic_states = 0;
        auto num_non_isomorphic_states = 0;
        auto states = GlobalFaithfulAbstractStateList(faithful_abstraction.get_num_states(), GlobalFaithfulAbstractState(-1, -1, -1));
        auto states_by_certificate = CertificateToStateIdMap {};
        for (size_t state_id = 0; state_id < faithful_abstraction.get_num_states(); ++state_id)
        {
            const auto& state = faithful_abstraction.get_states().at(state_id);
            auto it = certificate_to_global_state.find(state.get_certificate());

            if (it != certificate_to_global_state.end())
            {
                // Copy existing global state
                states.at(state_id) = it->second;
                states_by_certificate.emplace(it->first, state_id);
                ++num_isomorphic_states;
            }
            else
            {
                // Create new global state
                const auto new_global_state_id = certificate_to_global_state.size();
                auto new_global_state = GlobalFaithfulAbstractState(new_global_state_id, abstraction_id, state_id);
                certificate_to_global_state.emplace(state.get_certificate(), new_global_state);
                states.at(state_id) = new_global_state;
                states_by_certificate.emplace(state.get_certificate(), new_global_state_id);
                ++num_non_isomorphic_states;
            }
        }

        // Ensure that all global states were correctly initialized.
        assert(std::all_of(states.begin(),
                           states.end(),
                           [](const GlobalFaithfulAbstractState& global_state)
                           { return global_state.get_id() != -1 && global_state.get_abstraction_id() != -1 && global_state.get_abstract_state_id() != -1; }));

        // Constructor of GlobalFaithfulAbstraction requires this to come first.
        relevant_faithful_abstractions->push_back(std::move(faithful_abstraction));

        abstractions.push_back(GlobalFaithfulAbstraction(mark_true_goal_atoms,
                                                         use_unit_cost_one,
                                                         abstraction_id,
                                                         relevant_faithful_abstractions,
                                                         std::move(states),
                                                         std::move(states_by_certificate),
                                                         num_isomorphic_states,
                                                         num_non_isomorphic_states));
        ++abstraction_id;
    }

    return abstractions;
}

/**
 * Abstraction functionality
 */

double GlobalFaithfulAbstraction::get_goal_distance(State concrete_state)
{
    const auto& object_graph = m_object_graph_factory.create(concrete_state);
    object_graph.get_digraph().to_nauty_graph(m_nauty_graph);
    return get_goal_distances().at(m_states_by_certificate.at(
        Certificate(m_nauty_graph.compute_certificate(object_graph.get_lab(), object_graph.get_ptn()), object_graph.get_sorted_vertex_colors())));
}

/**
 * Extended functionality
 */

std::vector<double> GlobalFaithfulAbstraction::compute_shortest_distances_from_states(const StateIdList& abstract_states, bool forward) const
{
    return mimir::compute_shortest_distances_from_states(*this, abstract_states, forward);
}

std::vector<std::vector<double>> GlobalFaithfulAbstraction::compute_pairwise_shortest_state_distances(bool forward) const
{
    return mimir::compute_pairwise_shortest_state_distances(*this, forward);
}

/**
 * Getters
 */

// Meta data
const fs::path& GlobalFaithfulAbstraction::get_domain_filepath() const { return m_abstractions->at(m_id).get_domain_filepath(); }

const fs::path& GlobalFaithfulAbstraction::get_problem_filepath() const { return m_abstractions->at(m_id).get_problem_filepath(); }

// Memory
const FaithfulAbstractionList& GlobalFaithfulAbstraction::get_abstractions() const { return *m_abstractions; }

// States
const GlobalFaithfulAbstractStateList& GlobalFaithfulAbstraction::get_states() const { return m_states; }

const CertificateToStateIdMap& GlobalFaithfulAbstraction::get_states_by_certificate() const { return m_states_by_certificate; }

StateId GlobalFaithfulAbstraction::get_initial_state() const { return m_abstractions->at(m_id).get_initial_state(); }

const StateIdSet& GlobalFaithfulAbstraction::get_goal_states() const { return m_abstractions->at(m_id).get_goal_states(); }

const StateIdSet& GlobalFaithfulAbstraction::get_deadend_states() const { return m_abstractions->at(m_id).get_deadend_states(); }

size_t GlobalFaithfulAbstraction::get_num_states() const { return m_states.size(); }

size_t GlobalFaithfulAbstraction::get_num_goal_states() const { return m_abstractions->at(m_id).get_num_goal_states(); }

size_t GlobalFaithfulAbstraction::get_num_deadend_states() const { return m_abstractions->at(m_id).get_num_deadend_states(); }

size_t GlobalFaithfulAbstraction::get_num_isomorphic_states() const { return m_num_isomorphic_states; }

size_t GlobalFaithfulAbstraction::get_num_non_isomorphic_states() const { return m_num_non_isomorphic_states; }

// Transitions
size_t GlobalFaithfulAbstraction::get_num_transitions() const { return m_abstractions->at(m_id).get_num_transitions(); }

const std::vector<TransitionList>& GlobalFaithfulAbstraction::get_forward_transitions() const { return m_abstractions->at(m_id).get_forward_transitions(); }

const std::vector<TransitionList>& GlobalFaithfulAbstraction::get_backward_transitions() const { return m_abstractions->at(m_id).get_backward_transitions(); }

// Distances
const std::vector<double>& GlobalFaithfulAbstraction::get_goal_distances() const { return m_abstractions->at(m_id).get_goal_distances(); }

}
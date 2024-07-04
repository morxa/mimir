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

#include "mimir/datasets/transition.hpp"

namespace mimir
{

/**
 * Transition
 */

Transition::Transition(StateIndex src_state, StateIndex dst_state, GroundAction creating_action) :
    m_src_state(src_state),
    m_dst_state(dst_state),
    m_creating_action(creating_action)
{
}

bool Transition::operator==(const Transition& other) const
{
    if (this != &other)
    {
        return (m_src_state == other.m_src_state) && (m_dst_state == other.m_dst_state) && (m_creating_action == other.m_creating_action);
    }
    return true;
}

size_t Transition::hash() const { return loki::hash_combine(m_src_state, m_dst_state, m_creating_action.hash()); }

StateIndex Transition::get_src_state() const { return m_src_state; }

StateIndex Transition::get_dst_state() const { return m_dst_state; }

TransitionCost Transition::get_cost() const { return m_creating_action.get_cost(); }

GroundAction Transition::get_creating_action() const { return m_creating_action; }

/**
 * AbstractTransition
 */

AbstractTransition::AbstractTransition(StateIndex src_state, StateIndex dst_state, std::span<GroundAction> creating_actions) :
    m_src_state(src_state),
    m_dst_state(dst_state),
    m_creating_actions(creating_actions)
{
}

bool AbstractTransition::operator==(const AbstractTransition& other) const
{
    if (this != &other)
    {
        return (m_src_state == other.m_src_state)                                 //
               && (m_dst_state == other.m_dst_state)                              //
               && (m_creating_actions.size() == other.m_creating_actions.size())  //
               && std::equal(m_creating_actions.begin(), m_creating_actions.end(), other.m_creating_actions.begin());
    }
    return true;
}

size_t AbstractTransition::hash() const
{
    size_t creating_actions_hash = m_creating_actions.size();
    for (const auto& action : m_creating_actions)
    {
        loki::hash_combine(creating_actions_hash, action.hash());
    }
    return loki::hash_combine(m_src_state, m_dst_state, creating_actions_hash);
}

StateIndex AbstractTransition::get_src_state() const { return m_src_state; }

StateIndex AbstractTransition::get_dst_state() const { return m_dst_state; }

TransitionCost AbstractTransition::get_cost() const
{
    auto cost = std::numeric_limits<double>::max();

    std::for_each(m_creating_actions.begin(), m_creating_actions.end(), [&cost](const auto& action) { cost = std::min(cost, (double) action.get_cost()); });

    return cost;
}

std::span<GroundAction> AbstractTransition::get_creating_actions() const { return m_creating_actions; }

}
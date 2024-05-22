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

#ifndef MIMIR_SEARCH_ACTIONS_INTERFACE_HPP_
#define MIMIR_SEARCH_ACTIONS_INTERFACE_HPP_

#include "mimir/search/actions/tags.hpp"
#include "mimir/search/flat_types.hpp"

namespace mimir
{

/**
 * Interface class
 */
template<typename Derived>
class IActionBuilder
{
private:
    IActionBuilder() = default;
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    [[nodiscard]] uint32_t& get_id() { return self().get_id_impl(); }
    [[nodiscard]] int32_t& get_cost() { return self().get_cost_impl(); }
    [[nodiscard]] Action& get_action() { return self().get_action_impl(); }
    [[nodiscard]] FlatObjectListBuilder& get_objects() { return self().get_objects_impl(); }
};

/**
 * Interface class
 */
template<typename Derived>
class IActionView
{
private:
    IActionView() = default;
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    [[nodiscard]] uint32_t get_id() const { return self().get_id_impl(); }
    [[nodiscard]] int32_t get_cost() const { return self().get_cost_impl(); }
    [[nodiscard]] Action get_action() const { return self().get_action_impl(); }
    [[nodiscard]] FlatObjectList get_objects() const { return self().get_objects_impl(); }
};

}

#endif
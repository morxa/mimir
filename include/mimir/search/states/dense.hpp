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

#ifndef MIMIR_SEARCH_STATES_DENSE_HPP_
#define MIMIR_SEARCH_STATES_DENSE_HPP_

#include "mimir/common/concepts.hpp"
#include "mimir/common/printers.hpp"
#include "mimir/formalism/formalism.hpp"
#include "mimir/search/flat_types.hpp"
#include "mimir/search/states/interface.hpp"

#include <flatmemory/flatmemory.hpp>
#include <ostream>
#include <tuple>

namespace mimir
{
/**
 * Flatmemory types
 */
using FlatDenseStateLayout = flatmemory::Tuple<uint32_t, FlatBitsetLayout<Fluent>, FlatBitsetLayout<Derived>, Problem>;
using FlatDenseStateBuilder = flatmemory::Builder<FlatDenseStateLayout>;
using FlatDenseState = flatmemory::ConstView<FlatDenseStateLayout>;

// Only hash/compare the non-extended portion of a state, and the problem.
// The extended portion is computed automatically, when calling ssg.create_state(...)
struct FlatDenseStateHash
{
    size_t operator()(const FlatDenseState& view) const
    {
        const auto fluent_atoms = view.get<1>();
        const auto problem = view.get<3>();
        return loki::hash_combine(fluent_atoms.hash(), problem);
    }
};

struct FlatDenseStateEqual
{
    bool operator()(const FlatDenseState& view_left, const FlatDenseState& view_right) const
    {
        const auto fluent_atoms_left = view_left.get<1>();
        const auto fluent_atoms_right = view_right.get<1>();
        const auto problem_left = view_left.get<3>();
        const auto problem_right = view_right.get<3>();
        return (fluent_atoms_left == fluent_atoms_right) && (problem_left == problem_right);
    }
};

using FlatDenseStateSet = flatmemory::UnorderedSet<FlatDenseStateLayout, FlatDenseStateHash, FlatDenseStateEqual>;
using FlatDenseStateVector = flatmemory::FixedSizedTypeVector<FlatDenseStateLayout>;

/**
 * Implementation class
 */
/**
 * Implementation class
 */
template<>
class Builder<StateDispatcher<DenseStateTag>> :
    public IBuilder<Builder<StateDispatcher<DenseStateTag>>>,
    public IStateBuilder<Builder<StateDispatcher<DenseStateTag>>>
{
private:
    FlatDenseStateBuilder m_builder;

    /* Implement IBuilder interface */
    friend class IBuilder<Builder<StateDispatcher<DenseStateTag>>>;

    [[nodiscard]] FlatDenseStateBuilder& get_flatmemory_builder_impl() { return m_builder; }
    [[nodiscard]] const FlatDenseStateBuilder& get_flatmemory_builder_impl() const { return m_builder; }

    /* Implement IStateBuilder interface */
    friend class IStateBuilder<Builder<StateDispatcher<DenseStateTag>>>;

    [[nodiscard]] uint32_t& get_id_impl() { return m_builder.get<0>(); }

public:
    template<PredicateCategory P>
    [[nodiscard]] FlatBitsetBuilder<P>& get_atoms()
    {
        if constexpr (std::is_same_v<P, Static>)
        {
            static_assert(dependent_false<P>::value, "Modifying static grounded atoms is not allowed.");
        }
        else if constexpr (std::is_same_v<P, Fluent>)
        {
            return m_builder.get<1>();
        }
        else if constexpr (std::is_same_v<P, Derived>)
        {
            return m_builder.get<2>();
        }
        else
        {
            static_assert(dependent_false<P>::value, "Missing implementation for PredicateCategory.");
        }
    }

    [[nodiscard]] Problem& get_problem() { return m_builder.get<3>(); }
};

/**
 * Implementation class
 *
 * Reads the memory layout generated by the lifted state builder.
 */
template<>
class ConstView<StateDispatcher<DenseStateTag>> :
    public IConstView<ConstView<StateDispatcher<DenseStateTag>>>,
    public IStateView<ConstView<StateDispatcher<DenseStateTag>>>
{
private:
    FlatDenseState m_view;

    /* Implement IView interface */
    friend class IConstView<ConstView<StateDispatcher<DenseStateTag>>>;

    /// @brief Return true iff two states are equal.
    ///
    /// For states in same SSG, we know they are already unique.
    /// Hence, comparison of the buffer pointer suffices.
    /// For states in different SSG, buffer pointers are always different.
    /// Hence, comparison always returns false.
    [[nodiscard]] bool are_equal_impl(const ConstView& other) const { return m_view.buffer() == other.m_view.buffer(); }

    /// @brief Return a hash value for the state.
    ///
    /// Same argument from are_equal_impl applies.
    [[nodiscard]] size_t hash_impl() const { return loki::hash_combine(m_view.buffer()); }

    /* Implement IStateView interface */
    friend class IStateView<ConstView<StateDispatcher<DenseStateTag>>>;

    [[nodiscard]] uint32_t get_id_impl() const { return m_view.get<0>(); }

    [[nodiscard]] auto begin_impl() const { return get_atoms<Fluent>().begin(); }
    [[nodiscard]] auto end_impl() const { return get_atoms<Fluent>().end(); }

public:
    explicit ConstView(FlatDenseState view) : m_view(view) {}

    template<PredicateCategory P>
    [[nodiscard]] FlatBitset<P> get_atoms() const
    {
        if constexpr (std::is_same_v<P, Static>)
        {
            return get_problem()->get_static_initial_positive_atoms_bitset();
        }
        else if constexpr (std::is_same_v<P, Fluent>)
        {
            return m_view.get<1>();
        }
        else if constexpr (std::is_same_v<P, Derived>)
        {
            return m_view.get<2>();
        }
        else
        {
            static_assert(dependent_false<P>::value, "Missing implementation for PredicateCategory.");
        }
    }

    [[nodiscard]] Problem get_problem() const { return m_view.get<3>(); }

    template<PredicateCategory P>
    bool contains(const GroundAtom<P>& ground_atom) const
    {
        return get_atoms<P>().get(ground_atom->get_identifier());
    }

    template<PredicateCategory P>
    bool literal_holds(const GroundLiteral<P>& literal) const
    {
        return literal->is_negated() != contains(literal->get_atom());
    }

    template<PredicateCategory P>
    bool literals_hold(const GroundLiteralList<P>& literals) const
    {
        for (const auto& literal : literals)
        {
            if (!literal_holds(literal))
            {
                return false;
            }
        }

        return true;
    }
};

/**
 * Mimir types
 */

// TODO: differentiate registered and unregistered state can help us remove some duplicate code.
// We could use unregistered states internally to represent some unfinished state.

using DenseStateBuilder = Builder<StateDispatcher<DenseStateTag>>;
using DenseState = ConstView<StateDispatcher<DenseStateTag>>;
using DenseStateList = std::vector<DenseState>;

struct DenseStateHash
{
    size_t operator()(const DenseState& view) const { return view.hash(); }
};

using DenseStateSet = std::unordered_set<DenseState, DenseStateHash>;

/**
 * Pretty printing
 */

extern std::ostream& operator<<(std::ostream& os, const std::tuple<DenseState, const PDDLFactories&>& data);

}

#endif

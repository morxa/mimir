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

#include "mimir/common/printers.hpp"
#include "mimir/formalism/ground_atom.hpp"
#include "mimir/formalism/ground_literal.hpp"
#include "mimir/formalism/predicate.hpp"
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
using FlatDenseStateLayout = flatmemory::Tuple<uint32_t, FlatBitsetLayout, Problem>;
using FlatDenseStateBuilder = flatmemory::Builder<FlatDenseStateLayout>;
using FlatDenseState = flatmemory::ConstView<FlatDenseStateLayout>;

struct FlatDenseStateHash
{
    size_t operator()(const FlatDenseState& view) const
    {
        const auto bitset_view = view.get<1>();
        const auto problem = view.get<2>();
        return loki::hash_combine(bitset_view.hash(), problem);
    }
};

struct FlatDenseStateEqual
{
    bool operator()(const FlatDenseState& view_left, const FlatDenseState& view_right) const
    {
        const auto bitset_view_left = view_left.get<1>();
        const auto bitset_view_right = view_right.get<1>();
        const auto problem_left = view_left.get<2>();
        const auto problem_right = view_right.get<2>();
        return (bitset_view_left == bitset_view_right) && (problem_left == problem_right);
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
    [[nodiscard]] FlatBitsetBuilder& get_atoms_bitset() { return m_builder.get<1>(); }
    [[nodiscard]] Problem& get_problem() { return m_builder.get<2>(); }
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

    [[nodiscard]] bool are_equal_impl(const ConstView& other) const { return get_atoms_bitset() == other.get_atoms_bitset(); }

    [[nodiscard]] size_t hash_impl() const { return get_atoms_bitset().hash(); }

    /* Implement IStateView interface */
    friend class IStateView<ConstView<StateDispatcher<DenseStateTag>>>;

    [[nodiscard]] uint32_t get_id_impl() const { return m_view.get<0>(); }

    [[nodiscard]] auto begin_impl() const { return get_atoms_bitset().begin(); }
    [[nodiscard]] auto end_impl() const { return get_atoms_bitset().end(); }

public:
    explicit ConstView(FlatDenseState view) : m_view(view) {}

    [[nodiscard]] FlatBitset get_atoms_bitset() const { return m_view.get<1>(); }
    [[nodiscard]] Problem get_problem() const { return m_view.get<2>(); }

    bool contains(const GroundAtom<FluentPredicateImpl>& ground_atom) const { return get_atoms_bitset().get(ground_atom->get_identifier()); }

    bool literal_holds(const GroundLiteral<FluentPredicateImpl>& literal) const { return literal->is_negated() != contains(literal->get_atom()); }

    bool literals_hold(const GroundLiteralList<FluentPredicateImpl>& literals) const
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

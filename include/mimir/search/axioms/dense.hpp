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

#ifndef MIMIR_SEARCH_AXIOMS_DENSE_HPP_
#define MIMIR_SEARCH_AXIOMS_DENSE_HPP_

#include "mimir/formalism/factories.hpp"
#include "mimir/search/actions/dense.hpp"
#include "mimir/search/axioms/interface.hpp"
#include "mimir/search/flat_types.hpp"
#include "mimir/search/states.hpp"

#include <cstdint>
#include <ostream>
#include <tuple>

namespace mimir
{

/**
 * Flatmemory types
 */
using FlatDenseAxiomLayout = flatmemory::Tuple<uint32_t,  //
                                               Axiom,
                                               FlatObjectListLayout,
                                               FlatBitsetLayout,
                                               FlatBitsetLayout,
                                               FlatBitsetLayout,
                                               FlatBitsetLayout,
                                               FlatSimpleEffect>;
using FlatDenseAxiomBuilder = flatmemory::Builder<FlatDenseAxiomLayout>;
using FlatDenseAxiom = flatmemory::ConstView<FlatDenseAxiomLayout>;
using FlatDenseAxiomVector = flatmemory::VariableSizedTypeVector<FlatDenseAxiomLayout>;

struct FlatDenseAxiomHash
{
    size_t operator()(const FlatDenseAxiom& view) const
    {
        const auto axiom = view.get<1>();
        const auto objects = view.get<2>();
        return loki::hash_combine(axiom, objects.hash());
    }
};

struct FlatDenseAxiomEqual
{
    bool operator()(const FlatDenseAxiom& view_left, const FlatDenseAxiom& view_right) const
    {
        const auto axiom_left = view_left.get<1>();
        const auto objects_left = view_left.get<2>();
        const auto axiom_right = view_right.get<1>();
        const auto objects_right = view_right.get<2>();
        return (axiom_left == axiom_right) && (objects_left == objects_right);
    }
};

using FlatDenseAxiomSet = flatmemory::UnorderedSet<FlatDenseAxiomLayout, FlatDenseAxiomHash, FlatDenseAxiomEqual>;

/**
 * Implementation class
 */
template<>
class Builder<AxiomDispatcher<DenseStateTag>> :
    public IBuilder<Builder<AxiomDispatcher<DenseStateTag>>>,
    public IAxiomBuilder<Builder<AxiomDispatcher<DenseStateTag>>>
{
private:
    FlatDenseAxiomBuilder m_builder;

    /* Implement IBuilder interface */
    friend class IBuilder<Builder<AxiomDispatcher<DenseStateTag>>>;

    [[nodiscard]] FlatDenseAxiomBuilder& get_flatmemory_builder_impl() { return m_builder; }
    [[nodiscard]] const FlatDenseAxiomBuilder& get_flatmemory_builder_impl() const { return m_builder; }

    /* Implement IAxiomBuilder interface */
    friend class IAxiomBuilder<Builder<AxiomDispatcher<DenseStateTag>>>;

    [[nodiscard]] uint32_t& get_id_impl() { return m_builder.get<0>(); }
    [[nodiscard]] Axiom& get_axiom_impl() { return m_builder.get<1>(); }
    [[nodiscard]] FlatObjectListBuilder& get_objects_impl() { return m_builder.get<2>(); }

public:
    /* Precondition */
    [[nodiscard]] FlatBitsetBuilder& get_applicability_positive_precondition_bitset() { return m_builder.get<3>(); }
    [[nodiscard]] FlatBitsetBuilder& get_applicability_negative_precondition_bitset() { return m_builder.get<4>(); }
    [[nodiscard]] FlatBitsetBuilder& get_applicability_positive_static_precondition_bitset() { return m_builder.get<5>(); }
    [[nodiscard]] FlatBitsetBuilder& get_applicability_negative_static_precondition_bitset() { return m_builder.get<6>(); }
    /* Simple effect */
    [[nodiscard]] FlatSimpleEffect& get_simple_effect() { return m_builder.get<7>(); }
};

/**
 * Implementation class
 *
 * Reads the memory layout generated by the search node builder.
 */
template<>
class ConstView<AxiomDispatcher<DenseStateTag>> :
    public IConstView<ConstView<AxiomDispatcher<DenseStateTag>>>,
    public IAxiomView<ConstView<AxiomDispatcher<DenseStateTag>>>
{
private:
    using DenseState = ConstView<StateDispatcher<DenseStateTag>>;

    FlatDenseAxiom m_view;

    /* Implement IView interface: */
    friend class IConstView<ConstView<AxiomDispatcher<DenseStateTag>>>;

    /// @brief Compute equality based on the lifted action and the objects assigned to the parameters.
    [[nodiscard]] bool are_equal_impl(const ConstView& other) const { return get_axiom() == other.get_axiom() && get_objects() == other.get_objects(); }

    /// @brief Compute hash based on the lifted action and the objects assigned to the parameters.
    [[nodiscard]] size_t hash_impl() const { return loki::hash_combine(get_axiom(), get_objects().hash()); }

    /* Implement IAxiomView interface */
    friend class IAxiomView<ConstView<AxiomDispatcher<DenseStateTag>>>;

    [[nodiscard]] uint32_t get_id_impl() const { return m_view.get<0>(); }
    [[nodiscard]] Axiom get_axiom_impl() const { return m_view.get<1>(); }
    [[nodiscard]] FlatObjectList get_objects_impl() const { return m_view.get<2>(); }

public:
    /// @brief Create a view on a Axiom.
    explicit ConstView(FlatDenseAxiom view) : m_view(view) {}

    /* Precondition */
    [[nodiscard]] FlatBitset get_applicability_positive_precondition_bitset() const { return m_view.get<3>(); }
    [[nodiscard]] FlatBitset get_applicability_negative_precondition_bitset() const { return m_view.get<4>(); }
    [[nodiscard]] FlatBitset get_applicability_positive_static_precondition_bitset() const { return m_view.get<5>(); }
    [[nodiscard]] FlatBitset get_applicability_negative_static_precondition_bitset() const { return m_view.get<6>(); }
    /* Effect*/
    [[nodiscard]] FlatSimpleEffect get_simple_effect() const { return m_view.get<7>(); }

    template<flatmemory::IsBitset Bitset1, flatmemory::IsBitset Bitset2, flatmemory::IsBitset Bitset3>
    [[nodiscard]] bool is_applicable(const Bitset1 state_bitset, const Bitset2 static_positive_bitset, const Bitset3 static_negative_bitset) const
    {
        return state_bitset.is_superseteq(get_applicability_positive_precondition_bitset())
               && static_positive_bitset.is_superseteq(get_applicability_positive_static_precondition_bitset())
               && state_bitset.are_disjoint(get_applicability_negative_precondition_bitset())
               && static_negative_bitset.are_disjoint(get_applicability_negative_static_precondition_bitset());
    }

    template<flatmemory::IsBitset Bitset>
    [[nodiscard]] bool is_statically_applicable(const Bitset static_negative_bitset) const
    {
        // positive atoms are a superset in the state
        return static_negative_bitset.are_disjoint(get_applicability_negative_static_precondition_bitset());
    }
};

/**
 * Mimir types
 */

using DenseGroundAxiomBuilder = Builder<AxiomDispatcher<DenseStateTag>>;
using DenseGroundAxiom = ConstView<AxiomDispatcher<DenseStateTag>>;
using DenseGroundAxiomList = std::vector<DenseGroundAxiom>;
using DenseGroundAxiomSet = std::unordered_set<DenseGroundAxiom, loki::Hash<DenseGroundAxiom>, loki::EqualTo<DenseGroundAxiom>>;

/**
 * Translations
 */

extern DenseGroundAxiomList to_ground_axioms(const FlatDenseAxiomSet& flat_axioms);

/**
 * Pretty printing
 */

extern std::ostream& operator<<(std::ostream& os, const std::tuple<DenseGroundAxiom, const PDDLFactories&>& data);
}

#endif

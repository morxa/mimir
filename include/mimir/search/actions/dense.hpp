#ifndef MIMIR_SEARCH_ACTIONS_DENSE_HPP_
#define MIMIR_SEARCH_ACTIONS_DENSE_HPP_

#include "mimir/common/hash.hpp"
#include "mimir/search/actions/interface.hpp"
#include "mimir/search/type_traits.hpp"
#include "mimir/search/types.hpp"

namespace mimir
{
/**
 * Flatmemory types
 */
using DenseActionLayout = flatmemory::Tuple<int32_t, int32_t, Action, ObjectListLayout, BitsetLayout, BitsetLayout, BitsetLayout, BitsetLayout>;
using DenseActionBuilder = flatmemory::Builder<DenseActionLayout>;
using ConstDenseActionView = flatmemory::ConstView<DenseActionLayout>;
using DenseActionVector = flatmemory::VariableSizedTypeVector<DenseActionLayout>;

/**
 * Implementation class
 */
template<>
class Builder<ActionDispatcher<DenseStateTag>> :
    public IBuilder<Builder<ActionDispatcher<DenseStateTag>>>,
    public IActionBuilder<Builder<ActionDispatcher<DenseStateTag>>>
{
private:
    DenseActionBuilder m_builder;

    /* Implement IBuilder interface */
    friend class IBuilder<Builder<ActionDispatcher<DenseStateTag>>>;

    [[nodiscard]] DenseActionBuilder& get_flatmemory_builder_impl() { return m_builder; }
    [[nodiscard]] const DenseActionBuilder& get_flatmemory_builder_impl() const { return m_builder; }

    /* Implement IActionBuilder interface */
    friend class IActionBuilder<Builder<ActionDispatcher<DenseStateTag>>>;

public:
    /// @brief Modify the data, call finish, then copy the buffer to a container and use its returned view.
    [[nodiscard]] int32_t& get_id() { return m_builder.get<0>(); }
    [[nodiscard]] int32_t& get_cost() { return m_builder.get<1>(); }
    [[nodiscard]] Action& get_action() { return m_builder.get<2>(); }
    [[nodiscard]] ObjectListBuilder& get_objects() { return m_builder.get<3>(); }
    [[nodiscard]] BitsetBuilder& get_applicability_positive_precondition_bitset() { return m_builder.get<4>(); }
    [[nodiscard]] BitsetBuilder& get_applicability_negative_precondition_bitset() { return m_builder.get<5>(); }
    [[nodiscard]] BitsetBuilder& get_unconditional_positive_effect_bitset() { return m_builder.get<6>(); }
    [[nodiscard]] BitsetBuilder& get_unconditional_negative_effect_bitset() { return m_builder.get<7>(); }
};

/**
 * Implementation class
 *
 * Reads the memory layout generated by the search node builder.
 */
template<>
class ConstView<ActionDispatcher<DenseStateTag>> :
    public IConstView<ConstView<ActionDispatcher<DenseStateTag>>>,
    public IActionView<ConstView<ActionDispatcher<DenseStateTag>>>
{
private:
    using ConstStateView = ConstView<StateDispatcher<DenseStateTag>>;

    ConstDenseActionView m_view;

    /* Implement IView interface: */
    friend class IConstView<ConstView<ActionDispatcher<DenseStateTag>>>;

    /// @brief Compute equality based on the lifted action and the objects assigned to the parameters.
    [[nodiscard]] bool are_equal_impl(const ConstView& other) const { return get_action() == other.get_action() && get_objects() == other.get_objects(); }

    /// @brief Compute hash based on the lifted action and the objects assigned to the parameters.
    [[nodiscard]] size_t hash_impl() const { return hash_combine(get_action(), get_objects().hash()); }

    /* Implement IActionView interface */
    friend class IActionView<ConstView<ActionDispatcher<DenseStateTag>>>;

public:
    /// @brief Create a view on a DefaultAction.
    explicit ConstView(ConstDenseActionView view) : m_view(view) {}

    [[nodiscard]] int32_t get_id() const { return m_view.get<0>(); }
    [[nodiscard]] int32_t get_cost() const { return m_view.get<1>(); }
    [[nodiscard]] Action get_action() const { return m_view.get<2>(); }
    [[nodiscard]] ConstObjectListView get_objects() const { return m_view.get<3>(); }
    [[nodiscard]] ConstBitsetView get_applicability_positive_precondition_bitset() const { return m_view.get<4>(); }
    [[nodiscard]] ConstBitsetView get_applicability_negative_precondition_bitset() const { return m_view.get<5>(); }
    [[nodiscard]] ConstBitsetView get_unconditional_positive_effect_bitset() const { return m_view.get<6>(); };
    [[nodiscard]] ConstBitsetView get_unconditional_negative_effect_bitset() const { return m_view.get<7>(); };

    [[nodiscard]] bool is_applicable(ConstStateView state) const
    {
        const auto state_bitset = state.get_atoms_bitset();
        return state_bitset.is_superseteq(get_applicability_positive_precondition_bitset())
               && state_bitset.are_disjoint(get_applicability_negative_precondition_bitset());
    }
};

/**
 * Mimir types
 */
using DenseActionBuilderProxy = Builder<ActionDispatcher<DenseStateTag>>;

using ConstDenseActionViewProxy = ConstView<ActionDispatcher<DenseStateTag>>;

}

#endif

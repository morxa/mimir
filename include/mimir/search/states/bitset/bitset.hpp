#ifndef MIMIR_SEARCH_STATES_BITSET_BITSET_HPP_
#define MIMIR_SEARCH_STATES_BITSET_BITSET_HPP_

#include "interface.hpp"

#include "../../../buffer/flatbuffers/state-bitset-grounded_generated.h"


namespace mimir
{

/**
 * Implementation class
*/
template<IsPlanningModeTag P>
class Builder<StateDispatcher<BitsetStateTag, P>>
    : public IBuilderBase<Builder<StateDispatcher<BitsetStateTag, P>>>
    , public IStateBuilder<Builder<StateDispatcher<BitsetStateTag, P>>>
    , public IBitsetStateBuilder<Builder<StateDispatcher<BitsetStateTag, P>>>
{
private:
    flatbuffers::FlatBufferBuilder m_flatbuffers_builder;

    uint32_t m_id;
    Bitset<uint64_t> m_atoms_bitset;

    /* Implement IBuilderBase interface */
    template<typename>
    friend class IBuilderBase;

    void finish_impl() {
        // Genenerate nested data first.
        auto created_atoms_vec = this->m_flatbuffers_builder.CreateVector(m_atoms_bitset.get_data());
        auto bitset = CreateBitsetFlat(m_flatbuffers_builder, m_atoms_bitset.get_data().size(), created_atoms_vec);
        // Generate state data.
        auto builder = StateBitsetGroundedFlatBuilder(this->m_flatbuffers_builder);
        builder.add_id(m_id);
        builder.add_atoms(bitset);
        this->m_flatbuffers_builder.FinishSizePrefixed(builder.Finish());
    }

    void clear_impl() {
        m_flatbuffers_builder.Clear();
        m_atoms_bitset.clear();
    }

    [[nodiscard]] uint8_t* get_buffer_pointer_impl() { return m_flatbuffers_builder.GetBufferPointer(); }
    [[nodiscard]] const uint8_t* get_buffer_pointer_impl() const { return m_flatbuffers_builder.GetBufferPointer(); }
    [[nodiscard]] uint32_t get_size_impl() const { return *reinterpret_cast<const flatbuffers::uoffset_t*>(this->get_buffer_pointer()) + sizeof(flatbuffers::uoffset_t); }


    /* Implement IStateBuilder interface */
    template<typename>
    friend class IStateBuilder;

    void set_id_impl(uint32_t id) { m_id = id; }


    /* Implement IBitsetStateBuilder interface */
    template<typename>
    friend class IBitsetStateBuilder;

    [[nodiscard]] Bitset<uint64_t>& get_atoms_bitset_impl() { return m_atoms_bitset; }
};


/**
 * Implementation class
 *
 * Reads the memory layout generated by the lifted state builder.
*/
template<IsPlanningModeTag P>
class View<StateDispatcher<BitsetStateTag, P>>
    : public ViewBase<View<StateDispatcher<BitsetStateTag, P>>>
    , public IStateView<View<StateDispatcher<BitsetStateTag, P>>>
    , public IBitsetStateView<View<StateDispatcher<BitsetStateTag, P>>>
{
private:
    const StateBitsetGroundedFlat* m_flatbuffers_view;

    /* Implement ViewBase interface */
    template<typename>
    friend class ViewBase;

    [[nodiscard]] bool are_equal_impl(const View& other) const {
        assert(m_flatbuffers_view);
        if (this != &other) {
            return this->get_atoms_bitset() == other.get_atoms_bitset();
        }
        return true;
    }

    /// @brief Hash the representative data.
    [[nodiscard]] size_t hash_impl() const {
        assert(m_flatbuffers_view);
        return this->get_atoms_bitset().hash();
    }


    /* Implement IStateView interface */
    template<typename>
    friend class IStateView;

    [[nodiscard]] uint32_t get_id_impl() const {
        assert(m_flatbuffers_view);
        return m_flatbuffers_view->id();
    }


    /* Implement IBitsetStateView interface*/
    template<typename>
    friend class IBitsetStateView;

    [[nodiscard]] BitsetView get_atoms_bitset_impl() const {
        assert(m_flatbuffers_view);
        return BitsetView(m_flatbuffers_view->atoms());
    }

public:
    explicit View(uint8_t* data)
        : ViewBase<View<StateDispatcher<BitsetStateTag, P>>>(data)
        , m_flatbuffers_view(data ? GetSizePrefixedStateBitsetGroundedFlat(reinterpret_cast<void*>(data)) : nullptr) { }
};


}

#endif
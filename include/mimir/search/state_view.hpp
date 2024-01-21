#ifndef MIMIR_SEARCH_STATE_VIEW_HPP_
#define MIMIR_SEARCH_STATE_VIEW_HPP_

#include "config.hpp"
#include "state.hpp"
#include "type_traits.hpp"

#include "../buffer/view_base.hpp"

#include <cstdint>
#include <iostream>


namespace mimir {

/**
 * Interface class
*/
template<typename Derived>
class StateViewBase {
private:
    using C = typename TypeTraits<Derived>::ConfigType;

    StateViewBase() = default;
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    /* Mutable getters. */

    /* Immutable getters. */
    state_id_type get_id() const { return self().get_id_impl(); }
};


/**
 * Implementation class.
 *
 * We provide specialized implementations for
 * - Grounded in grounded/state_view.hpp
 * - Lifted in lifted/state_view.hpp
*/
template<Config C>
class View<State<C>> : public ViewBase<View<State<C>>>, public StateViewBase<View<State<C>>> { };


/**
 * Type traits
*/
template<Config C>
struct TypeTraits<View<State<C>>> {
    using ConfigType = C;
};


} // namespace mimir



#endif  // MIMIR_SEARCH_SEARCH_NODE_VIEW_HPP_

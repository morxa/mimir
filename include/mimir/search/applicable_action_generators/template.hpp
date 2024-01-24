#ifndef MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_TEMPLATE_HPP_
#define MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_TEMPLATE_HPP_

#include "../actions.hpp"
#include "../states.hpp"
#include "../type_traits.hpp"

#include "../../formalism/problem/declarations.hpp"


namespace mimir
{

/**
 * Interface class.
*/
template<typename Derived>
class AAGBase : public UncopyableMixin<AAGBase<Derived>> {
private:
    using P = typename TypeTraits<Derived>::PlanningModeTag;
    using S = typename TypeTraits<Derived>::StateTag;
    using A = typename TypeTraits<Derived>::ActionTag;

    using StateView = View<WrappedStateTag<S, P>>;
    using ActionView = View<WrappedActionTag<A, P, S>>;


    AAGBase() = default;
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

public:
    /// @brief Generate all applicable actions for a given state.
    void generate_applicable_actions(StateView state, std::vector<ActionView>& out_applicable_actions) {
        self().generate_applicable_actions_impl(state, out_applicable_actions);
    }
};


/**
 * ID dispatch base class.
*/
struct AAGBaseTag {};

template<typename DerivedTag>
concept IsAAGTag = std::derived_from<DerivedTag, AAGBaseTag>;


/**
 * Wrapper class.
 *
 * Wrap the tag and the planning mode to be able use a given planning mode.
*/
template<IsAAGTag AAG, IsPlanningModeTag P, IsStateTag S, IsActionTag A>
struct WrappedAAGTag {};

template<typename T>
struct is_wrapped_aag_tag : std::false_type {};

template<IsAAGTag AAG, IsPlanningModeTag P, IsStateTag S, IsActionTag A>
struct is_wrapped_aag_tag<WrappedAAGTag<AAG, P, S, A>> : std::true_type {};

template<typename T>
concept IsWrappedAAGTag = is_wrapped_aag_tag<T>::value;


/**
 * General implementation class.
 *
 * Spezialize the wrapped tag to provide your own implementation of a applicable action generator.
*/
template<IsWrappedAAGTag A>
class AAG : public AAGBase<AAG<A>> { };



}  // namespace mimir

#endif  // MIMIR_SEARCH_APPLICABLE_ACTION_GENERATORS_TEMPLATE_HPP_

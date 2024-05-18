#include "mimir/search/actions/dense.hpp"

#include "mimir/common/printers.hpp"
#include "mimir/formalism/factories.hpp"

#include <flatmemory/flatmemory.hpp>
#include <ostream>
#include <tuple>

namespace mimir
{

std::ostream& operator<<(std::ostream& os, const std::tuple<FlatSimpleEffect, const PDDLFactories&>& data)
{
    const auto [simple_effect, pddl_factories] = data;

    const auto& ground_atom = pddl_factories.get_ground_atom(simple_effect.atom_id);

    if (simple_effect.is_negated)
    {
        os << "(not ";
    }

    os << *ground_atom;

    if (simple_effect.is_negated)
    {
        os << ")";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const std::tuple<DenseGroundAction, const PDDLFactories&>& data)
{
    const auto [action, pddl_factories] = data;

    auto positive_precondition_bitset = action.get_applicability_positive_precondition_bitset();
    auto negative_precondition_bitset = action.get_applicability_negative_precondition_bitset();
    auto positive_effect_bitset = action.get_unconditional_positive_effect_bitset();
    auto negative_effect_bitset = action.get_unconditional_negative_effect_bitset();
    auto positive_conditional_condition_bitsets = action.get_conditional_positive_precondition_bitsets();
    auto negative_conditional_condition_bitsets = action.get_conditional_negative_precondition_bitsets();
    auto conditional_effects = action.get_conditional_effects();

    auto positive_precondition = GroundAtomList {};
    auto negative_precondition = GroundAtomList {};
    auto positive_simple_effects = GroundAtomList {};
    auto negative_simple_effects = GroundAtomList {};
    auto positive_conditional_preconditions = std::vector<GroundAtomList> {};
    auto negative_conditional_preconditions = std::vector<GroundAtomList> {};

    pddl_factories.get_ground_atoms(positive_precondition_bitset, positive_precondition);
    pddl_factories.get_ground_atoms(negative_precondition_bitset, negative_precondition);
    pddl_factories.get_ground_atoms(positive_effect_bitset, positive_simple_effects);
    pddl_factories.get_ground_atoms(negative_effect_bitset, negative_simple_effects);

    const auto num_conditional_effects = action.get_conditional_effects().size();
    positive_conditional_preconditions.resize(num_conditional_effects);
    negative_conditional_preconditions.resize(num_conditional_effects);
    for (size_t i = 0; i < num_conditional_effects; ++i)
    {
        pddl_factories.get_ground_atoms(positive_conditional_condition_bitsets[i], positive_conditional_preconditions[i]);
        pddl_factories.get_ground_atoms(negative_conditional_condition_bitsets[i], negative_conditional_preconditions[i]);
    }

    os << "Action("
       << "id=" << action.get_id() << ", "
       << "name=" << action.get_action()->get_name() << ", "
       << "positive precondition=" << positive_precondition << ", "
       << "negative precondition=" << negative_precondition << ", "
       << "simple_delete=" << negative_simple_effects << ", "
       << "simple_add=" << positive_simple_effects << ", "
       << "conditional_effects=[";
    for (size_t i = 0; i < num_conditional_effects; ++i)
    {
        os << "[positive precondition=" << positive_conditional_preconditions[i] << ", "
           << "negative precondition=" << negative_conditional_preconditions[i] << ", "
           << "effect=" << std::make_tuple(conditional_effects[i], std::cref(pddl_factories)) << "], ";
    }
    os << "])";

    return os;
}

std::ostream& operator<<(std::ostream& os, const DenseGroundAction& action)
{
    os << "(" << action.get_action()->get_name();
    for (const auto& obj : action.get_objects())
    {
        os << " " << obj->get_name();
    }
    os << ")";
    return os;
}

}
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

#include "../algorithms/murmurhash3.hpp"
#include "help_functions.hpp"
#include "problem.hpp"
#include "state.hpp"

#include <algorithm>
#include <cassert>

namespace formalism
{
    inline std::size_t compute_state_hash(const boost::dynamic_bitset<>& bitset, const formalism::ProblemDescription& problem)
    {
        // TODO: Preferably, we want to use MurmurHash3 directly on the underlying data of the bitset, but only up until the last set bit. However,
        // boost::dynamic_bitset does not provide an efficient way of achieving this. For now, we create an array on the stack and fill it with the set
        // positions.

        const auto num_atoms = bitset.count();

        std::size_t index = 0;
        std::size_t indices[num_atoms];

        auto position = bitset.find_first();
        while (position < bitset.npos)
        {
            indices[index] = position;
            position = bitset.find_next(position);
            ++index;
        }

        int64_t hash[2];
        MurmurHash3_x64_128(indices, num_atoms * sizeof(std::size_t), 0, hash);
        return static_cast<std::size_t>(hash[0] + 0x9e3779b9 + (hash[1] << 6) + (hash[1] >> 2));
    }

    StateImpl::StateImpl(const boost::dynamic_bitset<>& bitset, const formalism::ProblemDescription& problem) :
        bitset_(bitset),
        problem_(problem),
        hash_(compute_state_hash(bitset, problem))
    {
    }

    StateImpl::StateImpl(const std::vector<uint32_t>& ranks, const formalism::ProblemDescription& problem) :
        bitset_(*std::max_element(ranks.cbegin(), ranks.cend()) + 1),
        problem_(problem),
        hash_(0)
    {
        for (auto rank : ranks)
        {
            bitset_.set(rank, true);
        }

        hash_ = compute_state_hash(bitset_, problem);
    }

    StateImpl::StateImpl() : bitset_(), problem_(nullptr), hash_(0) {}

    StateImpl::StateImpl(const formalism::AtomList& atoms, const formalism::ProblemDescription& problem) :
        bitset_(problem->num_ranks()),
        problem_(problem),
        hash_(0)
    {
        for (const auto& atom : atoms)
        {
            const auto rank = problem->get_rank(atom);

            if (rank >= bitset_.size())
            {
                bitset_.resize(rank + 1);
            }

            bitset_.set(rank, true);
        }

        hash_ = compute_state_hash(bitset_, problem_);
    }

    StateImpl::StateImpl(const formalism::AtomSet& atoms, const formalism::ProblemDescription& problem) : bitset_(), problem_(problem), hash_(0)
    {
        for (const auto& atom : atoms)
        {
            const auto rank = problem->get_rank(atom);

            if (rank >= bitset_.size())
            {
                bitset_.resize(rank + 1);
            }

            bitset_.set(rank, true);
        }

        hash_ = compute_state_hash(bitset_, problem_);
    }

    State create_state() { return std::make_shared<formalism::StateImpl>(); }

    State create_state(const formalism::AtomList& atoms, formalism::ProblemDescription problem)
    {
        return std::make_shared<formalism::StateImpl>(atoms, problem);
    }

    State create_state(const formalism::AtomSet& atoms, formalism::ProblemDescription problem)
    {
        return std::make_shared<formalism::StateImpl>(atoms, problem);
    }

    void resize_to_same_length(boost::dynamic_bitset<>& lhs, boost::dynamic_bitset<>& rhs)
    {
        const auto lhs_length = lhs.size();
        const auto rhs_length = rhs.size();

        if (lhs_length < rhs_length)
        {
            lhs.resize(rhs_length);
        }
        else if (lhs_length > rhs_length)
        {
            rhs.resize(lhs_length);
        }
    }

    inline bool StateImpl::operator<(const StateImpl& other) const
    {
        if (problem_ != other.problem_)
        {
            return problem_ < other.problem_;
        }

        resize_to_same_length(this->bitset_, other.bitset_);
        return bitset_ < other.bitset_;
    }

    inline bool StateImpl::operator==(const StateImpl& other) const
    {
        if (hash_ != other.hash_)
        {
            return false;
        }

        if (problem_.get() != other.problem_.get())
        {
            return false;
        }

        resize_to_same_length(this->bitset_, other.bitset_);
        return bitset_ == other.bitset_;
    }

    inline bool StateImpl::operator!=(const StateImpl& other) const { return !(this->operator==(other)); }

    formalism::AtomList StateImpl::get_atoms() const
    {
        formalism::AtomList atoms;

        auto position = bitset_.find_first();

        while (position < bitset_.npos)
        {
            const auto rank = static_cast<uint32_t>(position);
            atoms.emplace_back(problem_->get_atom(rank));
            position = bitset_.find_next(position);
        }

        return atoms;
    }

    formalism::AtomList StateImpl::get_static_atoms() const
    {
        formalism::AtomList atoms;

        auto position = bitset_.find_first();

        while (position < bitset_.npos)
        {
            const auto rank = static_cast<uint32_t>(position);

            if (problem_->is_static(rank))
            {
                atoms.emplace_back(problem_->get_atom(rank));
            }

            position = bitset_.find_next(position);
        }

        return atoms;
    }

    formalism::AtomList StateImpl::get_dynamic_atoms() const
    {
        formalism::AtomList atoms;

        auto position = bitset_.find_first();

        while (position < bitset_.npos)
        {
            const auto rank = static_cast<uint32_t>(position);

            if (problem_->is_dynamic(rank))
            {
                atoms.emplace_back(problem_->get_atom(rank));
            }

            position = bitset_.find_next(position);
        }

        return atoms;
    }

    std::vector<uint32_t> StateImpl::get_ranks() const
    {
        std::vector<uint32_t> ranks;

        auto position = bitset_.find_first();

        while (position < bitset_.npos)
        {
            const auto rank = static_cast<uint32_t>(position);
            ranks.emplace_back(rank);
            position = bitset_.find_next(position);
        }

        return ranks;
    }

    std::vector<uint32_t> StateImpl::get_static_ranks() const
    {
        std::vector<uint32_t> static_ranks;

        auto position = bitset_.find_first();

        while (position < bitset_.npos)
        {
            const auto rank = static_cast<uint32_t>(position);

            if (problem_->is_static(rank))
            {
                static_ranks.emplace_back(rank);
            }

            position = bitset_.find_next(position);
        }

        return static_ranks;
    }

    std::vector<uint32_t> StateImpl::get_dynamic_ranks() const
    {
        std::vector<uint32_t> static_ranks;

        auto position = bitset_.find_first();

        while (position < bitset_.npos)
        {
            const auto rank = static_cast<uint32_t>(position);

            if (problem_->is_dynamic(rank))
            {
                static_ranks.emplace_back(rank);
            }

            position = bitset_.find_next(position);
        }

        return static_ranks;
    }

    formalism::ProblemDescription StateImpl::get_problem() const { return problem_; }

    std::map<formalism::Predicate, formalism::AtomList> StateImpl::get_atoms_grouped_by_predicate() const
    {
        std::map<formalism::Predicate, formalism::AtomList> grouped_atoms;

        for (const auto& atom : get_atoms())
        {
            const auto& predicate = atom->predicate;

            if (grouped_atoms.find(predicate) == grouped_atoms.end())
            {
                grouped_atoms.insert(std::make_pair(predicate, formalism::AtomList()));
            }

            auto& atom_list = grouped_atoms.at(predicate);
            atom_list.push_back(atom);
        }

        return grouped_atoms;
    }

    std::pair<std::map<uint32_t, std::vector<uint32_t>>, std::map<uint32_t, std::pair<std::string, uint32_t>>>
    StateImpl::pack_object_ids_by_predicate_id(bool include_types, bool include_goal) const
    {
        std::map<uint32_t, std::vector<uint32_t>> packed_ids;
        std::map<uint32_t, std::pair<std::string, uint32_t>> id_to_name_arity;

        for (const auto& atom : get_atoms())
        {
            const auto& predicate = atom->predicate;

            if (packed_ids.find(predicate->id) == packed_ids.end())
            {
                packed_ids.insert(std::make_pair(predicate->id, std::vector<uint32_t>()));
                id_to_name_arity.insert(std::make_pair(predicate->id, std::make_pair(predicate->name, predicate->arity)));
            }

            auto& atom_list = packed_ids.at(predicate->id);

            for (const auto& object : atom->arguments)
            {
                atom_list.push_back(object->id);
            }
        }

        auto num_predicates = problem_->domain->predicates.size();

        if (include_types)
        {
            std::map<formalism::Type, uint32_t> type_ids;
            const auto& types = problem_->domain->types;

            for (const auto& type : types)
            {
                type_ids.insert(std::make_pair(type, num_predicates + type_ids.size()));
            }

            for (const auto& object : problem_->objects)
            {
                const auto object_id = object->id;
                auto type = object->type;

                while (type != nullptr)
                {
                    const auto predicate_id = type_ids[type];

                    if (packed_ids.find(predicate_id) == packed_ids.end())
                    {
                        packed_ids.insert(std::make_pair(predicate_id, std::vector<uint32_t>()));
                        id_to_name_arity.insert(std::make_pair(predicate_id, std::make_pair(type->name + "_type", 1)));
                    }

                    auto& atom_list = packed_ids.at(predicate_id);
                    atom_list.push_back(object_id);
                    type = type->base;
                }
            }

            num_predicates += types.size();
        }

        if (include_goal)
        {
            for (const auto& literal : problem_->goal)
            {
                if (literal->negated)
                {
                    throw std::invalid_argument("negated literal in the goal");
                }

                const auto atom = literal->atom;
                const auto predicate_id = num_predicates + atom->predicate->id;

                if (packed_ids.find(predicate_id) == packed_ids.end())
                {
                    packed_ids.insert(std::make_pair(predicate_id, std::vector<uint32_t>()));
                    id_to_name_arity.insert(std::make_pair(predicate_id, std::make_pair(atom->predicate->name + "_goal", atom->predicate->arity)));
                }

                auto& atom_list = packed_ids.at(predicate_id);

                for (const auto& object : atom->arguments)
                {
                    atom_list.push_back(object->id);
                }
            }
        }

        return std::make_pair(packed_ids, id_to_name_arity);
    }

    bool is_in_state(uint32_t rank, const formalism::State& state) { return (rank < state->bitset_.size()) && (state->bitset_.test(rank)); }

    bool is_in_state(const formalism::Atom& atom, const formalism::State& state) { return is_in_state(state->get_problem()->get_rank(atom), state); }

    bool is_applicable(const formalism::Action& action, const formalism::State& state)
    {
        if (static_cast<int32_t>(action->get_arguments().size()) != action->schema->arity)
        {
            throw std::runtime_error("is_applicable: action is not ground");
        }

        auto bitset = state->bitset_;
        resize_to_same_length(bitset, action->positive_precondition_bitset_);
        resize_to_same_length(bitset, action->negative_precondition_bitset_);
        resize_to_same_length(bitset, state->bitset_);
        bitset |= action->positive_precondition_bitset_;
        bitset &= ~action->negative_precondition_bitset_;
        return state->bitset_ == bitset;
    }

    formalism::State apply(const formalism::Action& action, const formalism::State& state)
    {
        auto bitset = state->bitset_;
        resize_to_same_length(bitset, action->positive_effect_bitset_);
        resize_to_same_length(bitset, action->negative_effect_bitset_);
        bitset &= ~action->negative_effect_bitset_;
        bitset |= action->positive_effect_bitset_;
        return std::make_shared<formalism::StateImpl>(std::move(bitset), state->problem_);
    }

    bool atoms_hold(const AtomList& atoms, const formalism::State& state)
    {
        for (const auto& atom : atoms)
        {
            if (!formalism::is_in_state(atom, state))
            {
                return false;
            }
        }

        return true;
    }

    bool literal_holds(const formalism::Literal& literal, const formalism::State& state)
    {
        return formalism::is_in_state(literal->atom, state) != literal->negated;
    }

    bool literals_hold(const LiteralList& literal_list, const formalism::State& state, std::size_t min_arity)
    {
        for (const auto& literal : literal_list)
        {
            const auto predicate_arity = literal->atom->predicate->arity;

            if ((predicate_arity >= min_arity) && !formalism::literal_holds(literal, state))
            {
                return false;
            }
        }

        return true;
    }

    std::ostream& operator<<(std::ostream& os, const formalism::State& state)
    {
        os << state->get_atoms();
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const formalism::StateList& states)
    {
        print_vector(os, states);
        return os;
    }
}  // namespace formalism

namespace std
{
    // Inject comparison and hash functions to make pointers behave appropriately with ordered and unordered datastructures
    std::size_t hash<formalism::State>::operator()(const formalism::State& state) const { return !state ? 0 : state->hash_; }

    bool less<formalism::State>::operator()(const formalism::State& left_state, const formalism::State& right_state) const
    {
        return *left_state < *right_state;
    }

    bool equal_to<formalism::State>::operator()(const formalism::State& left_state, const formalism::State& right_state) const
    {
        return *left_state == *right_state;
    }
}  // namespace std

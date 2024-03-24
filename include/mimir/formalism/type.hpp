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

#ifndef MIMIR_FORMALISM_TYPE_HPP_
#define MIMIR_FORMALISM_TYPE_HPP_

#include "mimir/formalism/declarations.hpp"

#include <loki/pddl/factory.hpp>
#include <loki/pddl/type.hpp>
#include <string>

namespace mimir
{
class TypeImpl : public loki::Base<TypeImpl>
{
private:
    std::string m_name;
    TypeList m_bases;

    // Below: add additional members if needed and initialize them in the constructor

    TypeImpl(int identifier, std::string name, TypeList bases = {});

    // Give access to the constructor.
    friend class loki::PDDLFactory<TypeImpl, loki::Hash<TypeImpl*>, loki::EqualTo<TypeImpl*>>;

    /// @brief Test for semantic equivalence
    bool is_structurally_equivalent_to_impl(const TypeImpl& other) const;
    size_t hash_impl() const;
    void str_impl(std::ostream& out, const loki::FormattingOptions& options) const;

    // Give access to the private interface implementations.
    friend class loki::Base<TypeImpl>;

public:
    const std::string& get_name() const;
    const TypeList& get_bases() const;
    bool is_subtype_of(const Type type) const;
    bool is_subtype_of(const TypeList& types) const;
};

bool is_any_subtype_of(const TypeList& subtypes, const TypeList& types);
}

#endif

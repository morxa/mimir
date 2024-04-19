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

#include "mimir/formalism/domain.hpp"

#include "mimir/formalism/action.hpp"
#include "mimir/formalism/axiom.hpp"
#include "mimir/formalism/function_skeleton.hpp"
#include "mimir/formalism/object.hpp"
#include "mimir/formalism/predicate.hpp"
#include "mimir/formalism/requirements.hpp"

#include <iostream>
#include <loki/loki.hpp>
#include <unordered_map>

using namespace std;

namespace mimir
{
DomainImpl::DomainImpl(int identifier,
                       std::string name,
                       Requirements requirements,
                       ObjectList constants,
                       PredicateList predicates,
                       PredicateList derived_predicates,
                       FunctionSkeletonList functions,
                       ActionList actions,
                       AxiomList axioms) :
    Base(identifier),
    m_name(std::move(name)),
    m_requirements(std::move(requirements)),
    m_constants(std::move(constants)),
    m_predicates(std::move(predicates)),
    m_derived_predicates(std::move(derived_predicates)),
    m_static_predicates(),
    m_fluent_predicates(),
    m_functions(std::move(functions)),
    m_actions(std::move(actions)),
    m_axioms(std::move(axioms))
{
    for (const auto& predicate : m_predicates)
    {
        if (any_affects(m_actions, predicate))
        {
            m_fluent_predicates.emplace_back(predicate);
        }
        else
        {
            m_static_predicates.emplace_back(predicate);
        }
    }
}

bool DomainImpl::is_structurally_equivalent_to_impl(const DomainImpl& other) const
{
    return (m_name == other.m_name) && (m_requirements == other.m_requirements)
           && (loki::get_sorted_vector(m_constants) == loki::get_sorted_vector(other.m_constants))
           && (loki::get_sorted_vector(m_predicates) == loki::get_sorted_vector(other.m_predicates))
           && (loki::get_sorted_vector(m_derived_predicates) == loki::get_sorted_vector(other.m_derived_predicates))
           && (loki::get_sorted_vector(m_functions) == loki::get_sorted_vector(other.m_functions))
           && (loki::get_sorted_vector(m_actions) == loki::get_sorted_vector(other.m_actions))
           && (loki::get_sorted_vector(m_axioms) == loki::get_sorted_vector(other.m_axioms));
}

size_t DomainImpl::hash_impl() const
{
    return loki::hash_combine(m_name,
                              m_requirements,
                              loki::hash_container(loki::get_sorted_vector(m_constants)),
                              loki::hash_container(loki::get_sorted_vector(m_predicates)),
                              loki::hash_container(loki::get_sorted_vector(m_derived_predicates)),
                              loki::hash_container(loki::get_sorted_vector(m_functions)),
                              loki::hash_container(loki::get_sorted_vector(m_actions)),
                              loki::hash_container(loki::get_sorted_vector(m_axioms)));
}

void DomainImpl::str_impl(std::ostream& out, const loki::FormattingOptions& options) const
{
    out << string(options.indent, ' ') << "(define (domain " << m_name << ")\n";
    auto nested_options = loki::FormattingOptions { options.indent + options.add_indent, options.add_indent };
    if (!m_requirements->get_requirements().empty())
    {
        out << string(nested_options.indent, ' ');
        m_requirements->str(out, nested_options);
        out << "\n";
    }
    if (!m_constants.empty())
    {
        out << string(nested_options.indent, ' ') << "(:constants ";
        for (size_t i = 0; i < m_constants.size(); ++i)
        {
            if (i != 0)
                out << " ";
            out << *m_constants[i];
        }
        out << ")\n";
    }
    if (!m_predicates.empty())
    {
        out << string(nested_options.indent, ' ') << "(:predicates ";
        for (size_t i = 0; i < m_predicates.size(); ++i)
        {
            if (i != 0)
                out << " ";
            m_predicates[i]->str(out, nested_options);
        }
        out << ")\n";
    }
    if (!m_derived_predicates.empty())
    {
        out << string(nested_options.indent, ' ') << "(:derived-predicates ";
        for (size_t i = 0; i < m_derived_predicates.size(); ++i)
        {
            if (i != 0)
                out << " ";
            m_derived_predicates[i]->str(out, nested_options);
        }
        out << ")\n";
    }
    if (!m_functions.empty())
    {
        out << string(nested_options.indent, ' ') << "(:functions ";
        for (size_t i = 0; i < m_functions.size(); ++i)
        {
            if (i != 0)
                out << " ";
            m_functions[i]->str(out, nested_options);
        }
    }

    for (const auto& action : m_actions)
    {
        action->str(out, nested_options);
    }

    for (const auto& axiom : m_axioms)
    {
        axiom->str(out, nested_options);
    }

    out << std::string(options.indent, ' ') << ")";
}

const std::string& DomainImpl::get_name() const { return m_name; }

const Requirements& DomainImpl::get_requirements() const { return m_requirements; }

const ObjectList& DomainImpl::get_constants() const { return m_constants; }

const PredicateList& DomainImpl::get_predicates() const { return m_predicates; }

const PredicateList& DomainImpl::get_static_predicates() const { return m_static_predicates; }

const PredicateList& DomainImpl::get_fluent_predicates() const { return m_fluent_predicates; }

const PredicateList& DomainImpl::get_derived_predicates() const { return m_derived_predicates; }

const FunctionSkeletonList& DomainImpl::get_functions() const { return m_functions; }

const ActionList& DomainImpl::get_actions() const { return m_actions; }

const AxiomList& DomainImpl::get_axioms() const { return m_axioms; }

}

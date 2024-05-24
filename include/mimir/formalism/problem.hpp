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

#ifndef MIMIR_FORMALISM_PROBLEM_HPP_
#define MIMIR_FORMALISM_PROBLEM_HPP_

#include "mimir/formalism/axiom.hpp"
#include "mimir/formalism/domain.hpp"
#include "mimir/formalism/ground_literal.hpp"
#include "mimir/formalism/metric.hpp"
#include "mimir/formalism/numeric_fluent.hpp"
#include "mimir/formalism/object.hpp"
#include "mimir/formalism/predicate.hpp"
#include "mimir/formalism/requirements.hpp"
#include "mimir/search/flat_types.hpp"

#include <loki/loki.hpp>
#include <optional>
#include <string>
#include <vector>

namespace mimir
{
class ProblemImpl : public loki::Base<ProblemImpl>
{
private:
    Domain m_domain;
    std::string m_name;
    Requirements m_requirements;
    ObjectList m_objects;
    FluentPredicateList m_derived_predicates;
    GroundLiteralList<StaticPredicateImpl> m_static_initial_literals;
    FlatBitsetBuilder m_static_initial_positive_atoms_builder;
    FlatBitsetBuilder m_static_initial_negative_atoms_builder;
    GroundLiteralList<FluentPredicateImpl> m_fluent_initial_literals;
    NumericFluentList m_numeric_fluents;
    GroundLiteralList<StaticPredicateImpl> m_static_goal_condition;
    GroundLiteralList<FluentPredicateImpl> m_fluent_goal_condition;
    std::optional<OptimizationMetric> m_optimization_metric;
    AxiomList m_axioms;

    // Below: add additional members if needed and initialize them in the constructor

    ProblemImpl(int identifier,
                Domain domain,
                std::string name,
                Requirements requirements,
                ObjectList objects,
                FluentPredicateList derived_predicates,
                GroundLiteralList<StaticPredicateImpl> static_initial_literals,
                GroundLiteralList<FluentPredicateImpl> fluent_initial_literals,
                NumericFluentList numeric_fluents,
                GroundLiteralList<StaticPredicateImpl> static_goal_condition,
                GroundLiteralList<FluentPredicateImpl> fluent_goal_condition,
                std::optional<OptimizationMetric> optimization_metric,
                AxiomList axioms);

    // Give access to the constructor.
    friend class loki::PDDLFactory<ProblemImpl, loki::Hash<ProblemImpl*>, loki::EqualTo<ProblemImpl*>>;

    /// @brief Test for semantic equivalence
    bool is_structurally_equivalent_to_impl(const ProblemImpl& other) const;
    size_t hash_impl() const;
    void str_impl(std::ostream& out, const loki::FormattingOptions& options) const;

    // Give access to the private interface implementations.
    friend class loki::Base<ProblemImpl>;

public:
    const Domain& get_domain() const;
    const std::string& get_name() const;
    const Requirements& get_requirements() const;
    const ObjectList& get_objects() const;
    const FluentPredicateList& get_derived_predicates() const;
    const GroundLiteralList<StaticPredicateImpl>& get_static_initial_literals() const;
    FlatBitset get_static_initial_positive_atoms_bitset() const;
    FlatBitset get_static_initial_negative_atoms_bitset() const;
    const GroundLiteralList<FluentPredicateImpl>& get_fluent_initial_literals() const;
    const NumericFluentList& get_numeric_fluents() const;
    const GroundLiteralList<StaticPredicateImpl>& get_static_goal_condition() const;
    const GroundLiteralList<FluentPredicateImpl>& get_fluent_goal_condition() const;
    const std::optional<OptimizationMetric>& get_optimization_metric() const;
    const AxiomList& get_axioms() const;
};

/**
 * Type aliases
 */

using Problem = const ProblemImpl*;
using ProblemList = std::vector<Problem>;

}

#endif

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

#ifndef MIMIR_GRAPHS_OBJECT_GRAPH_HPP_
#define MIMIR_GRAPHS_OBJECT_GRAPH_HPP_

#include "mimir/formalism/formalism.hpp"
#include "mimir/graphs/coloring.hpp"
#include "mimir/graphs/digraph.hpp"
#include "mimir/graphs/partitioning.hpp"
#include "mimir/search/flat_types.hpp"
#include "mimir/search/state.hpp"

#include <ostream>

namespace mimir
{
class ObjectGraphFactory;

class ObjectGraph
{
private:
    std::shared_ptr<const ProblemColorFunction> m_coloring_function;

    // Vertex colored graph, uses nauty's graph representation
    Digraph<DigraphEdge> m_digraph;
    ColorList m_vertex_colors;

    // Initial color histogram, needed for equivalence test
    // when using vertex partitioning to take color remapping into account.
    ColorList m_sorted_vertex_colors;
    Partitioning m_partitioning;

    friend class ObjectGraphFactory;

public:
    ObjectGraph(std::shared_ptr<const ProblemColorFunction> coloring_function);

    const std::shared_ptr<const ProblemColorFunction>& get_coloring_function() const;
    const Digraph<DigraphEdge>& get_digraph() const;
    const ColorList& get_vertex_colors() const;
    const ColorList& get_sorted_vertex_colors() const;
    const Partitioning& get_partitioning() const;
};

class ObjectGraphPruningStrategy
{
public:
    virtual ~ObjectGraphPruningStrategy() = default;

    virtual bool prune(const Object&) const { return false; };
    virtual bool prune(const GroundAtom<Static>) const { return false; };
    virtual bool prune(const GroundAtom<Fluent>) const { return false; };
    virtual bool prune(const GroundAtom<Derived>) const { return false; };
    virtual bool prune(const GroundLiteral<Static>) const { return false; }
    virtual bool prune(const GroundLiteral<Fluent>) const { return false; }
    virtual bool prune(const GroundLiteral<Derived>) const { return false; }
};

class ObjectGraphStaticPruningStrategy : public ObjectGraphPruningStrategy
{
public:
    ObjectGraphStaticPruningStrategy(FlatBitsetBuilder<> pruned_objects = FlatBitsetBuilder<>(),
                                     FlatBitsetBuilder<Static> pruned_ground_atoms = FlatBitsetBuilder<Static>(),
                                     FlatBitsetBuilder<Fluent> pruned_fluent_ground_atoms = FlatBitsetBuilder<Fluent>(),
                                     FlatBitsetBuilder<Derived> pruned_derived_ground_atoms = FlatBitsetBuilder<Derived>()) :
        m_pruned_objects(std::move(pruned_objects)),
        m_pruned_ground_atoms(std::move(pruned_ground_atoms)),
        m_pruned_fluent_ground_atoms(std::move(pruned_fluent_ground_atoms)),
        m_pruned_derived_ground_atoms(std::move(pruned_derived_ground_atoms))
    {
    }

    bool prune(const Object& object) const override { return m_pruned_objects.get(object->get_identifier()); }
    bool prune(const GroundAtom<Static> atom) const override { return m_pruned_ground_atoms.get(atom->get_identifier()); }
    bool prune(const GroundAtom<Fluent> atom) const override { return m_pruned_fluent_ground_atoms.get(atom->get_identifier()); }
    bool prune(const GroundAtom<Derived> atom) const override { return m_pruned_derived_ground_atoms.get(atom->get_identifier()); }
    bool prune(const GroundLiteral<Static> literal) const override { return m_pruned_ground_literals.get(literal->get_atom()->get_identifier()); }
    bool prune(const GroundLiteral<Fluent> literal) const override { return m_pruned_fluent_ground_literals.get(literal->get_atom()->get_identifier()); }
    bool prune(const GroundLiteral<Derived> literal) const override { return m_pruned_derived_ground_literals.get(literal->get_atom()->get_identifier()); }

    ObjectGraphStaticPruningStrategy& operator&=(const ObjectGraphStaticPruningStrategy& other)
    {
        m_pruned_objects &= other.m_pruned_objects;
        m_pruned_ground_atoms &= other.m_pruned_ground_atoms;
        m_pruned_fluent_ground_atoms &= other.m_pruned_fluent_ground_atoms;
        m_pruned_derived_ground_atoms &= other.m_pruned_derived_ground_atoms;
        m_pruned_ground_literals &= other.m_pruned_ground_literals;
        m_pruned_fluent_ground_literals &= other.m_pruned_fluent_ground_literals;
        m_pruned_derived_ground_literals &= other.m_pruned_derived_ground_literals;
        return *this;
    }

private:
    FlatBitsetBuilder<> m_pruned_objects;
    FlatBitsetBuilder<Static> m_pruned_ground_atoms;
    FlatBitsetBuilder<Fluent> m_pruned_fluent_ground_atoms;
    FlatBitsetBuilder<Derived> m_pruned_derived_ground_atoms;
    FlatBitsetBuilder<Static> m_pruned_ground_literals;
    FlatBitsetBuilder<Fluent> m_pruned_fluent_ground_literals;
    FlatBitsetBuilder<Derived> m_pruned_derived_ground_literals;
};

class ObjectGraphFactory
{
private:
    Problem m_problem;
    std::shared_ptr<PDDLFactories> m_pddl_factories;
    bool m_mark_true_goal_literals;

    std::shared_ptr<const ProblemColorFunction> m_coloring_function;

    ObjectGraph m_object_graph;

    // Temporaries for bookkeeping
    std::unordered_map<Object, int> m_object_to_vertex_index;
    std::vector<std::pair<int, int>> m_vertex_index_and_color;

    int add_object_graph_structures(Object object, int num_vertices);

    template<PredicateCategory P>
    int add_ground_atom_graph_structures(GroundAtom<P> atom, int num_vertices);

    template<PredicateCategory P>
    int add_ground_literal_graph_structures(State state, GroundLiteral<P> atom, int num_vertices);

public:
    ObjectGraphFactory(Problem problem, std::shared_ptr<PDDLFactories> pddl_factories, bool mark_true_goal_literals = false);

    /// @brief Create and return a reference to the object graph.
    const ObjectGraph& create(State state, const ObjectGraphPruningStrategy& pruning_strategy = ObjectGraphPruningStrategy());

    const std::shared_ptr<const ProblemColorFunction>& get_coloring_function() const;
};

/**
 * Pretty printing to dot representation
 */

extern std::ostream& operator<<(std::ostream& out, const ObjectGraph& object_graph);

/**
 * Implementations
 */

template<PredicateCategory P>
int ObjectGraphFactory::add_ground_atom_graph_structures(GroundAtom<P> atom, int num_vertices)
{
    for (size_t pos = 0; pos < atom->get_arity(); ++pos)
    {
        const auto vertex_color = m_coloring_function->get_color(atom, pos);
        m_object_graph.m_vertex_colors.push_back(vertex_color);
        m_object_graph.m_sorted_vertex_colors.push_back(vertex_color);
        m_object_graph.m_digraph.add_edge(num_vertices, m_object_to_vertex_index.at(atom->get_objects().at(pos)));
        if (pos > 0)
        {
            m_object_graph.m_digraph.add_edge(num_vertices - 1, num_vertices);
        }
        ++num_vertices;
    }
    return num_vertices;
}

template<PredicateCategory P>
int ObjectGraphFactory::add_ground_literal_graph_structures(State state, GroundLiteral<P> literal, int num_vertices)
{
    for (size_t pos = 0; pos < literal->get_atom()->get_arity(); ++pos)
    {
        const auto vertex_color = m_coloring_function->get_color(state, literal, pos, m_mark_true_goal_literals);
        m_object_graph.m_vertex_colors.push_back(vertex_color);
        m_object_graph.m_sorted_vertex_colors.push_back(vertex_color);
        m_object_graph.m_digraph.add_edge(num_vertices, m_object_to_vertex_index.at(literal->get_atom()->get_objects().at(pos)));
        if (pos > 0)
        {
            m_object_graph.m_digraph.add_edge(num_vertices - 1, num_vertices);
        }
        ++num_vertices;
    }
    return num_vertices;
}

}

#endif

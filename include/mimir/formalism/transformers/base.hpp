#ifndef MIMIR_FORMALISM_TRANSFORMERS_BASE_HPP_
#define MIMIR_FORMALISM_TRANSFORMERS_BASE_HPP_

#include "mimir/formalism/action.hpp"
#include "mimir/formalism/atom.hpp"
#include "mimir/formalism/axiom.hpp"
#include "mimir/formalism/domain.hpp"
#include "mimir/formalism/effect.hpp"
#include "mimir/formalism/factories.hpp"
#include "mimir/formalism/function.hpp"
#include "mimir/formalism/function_expressions.hpp"
#include "mimir/formalism/function_skeleton.hpp"
#include "mimir/formalism/ground_atom.hpp"
#include "mimir/formalism/ground_literal.hpp"
#include "mimir/formalism/literal.hpp"
#include "mimir/formalism/metric.hpp"
#include "mimir/formalism/numeric_fluent.hpp"
#include "mimir/formalism/object.hpp"
#include "mimir/formalism/parameter.hpp"
#include "mimir/formalism/predicate.hpp"
#include "mimir/formalism/problem.hpp"
#include "mimir/formalism/requirements.hpp"
#include "mimir/formalism/term.hpp"
#include "mimir/formalism/transformers/interface.hpp"
#include "mimir/formalism/variable.hpp"

#include <deque>
#include <loki/loki.hpp>
#include <unordered_map>

namespace mimir
{

/**
 * Base implementation recursively calls transform and caches the results.
 */
template<typename Derived>
class BaseTransformer : public ITransformer<BaseTransformer<Derived>>
{
private:
    BaseTransformer() = default;
    friend Derived;

    /// @brief Helper to cast to Derived.
    constexpr const auto& self() const { return static_cast<const Derived&>(*this); }
    constexpr auto& self() { return static_cast<Derived&>(*this); }

protected:
    PDDLFactories& m_pddl_factories;

    std::unordered_map<Requirements, Requirements> m_transformed_requirements;
    std::unordered_map<Object, Object> m_transformed_objects;
    std::unordered_map<Variable, Variable> m_transformed_variables;
    std::unordered_map<Term, Term> m_transformed_terms;
    std::unordered_map<Parameter, Parameter> m_transformed_parameters;
    std::unordered_map<Predicate, Predicate> m_transformed_predicates;
    std::unordered_map<Atom, Atom> m_transformed_atoms;
    std::unordered_map<GroundAtom, GroundAtom> m_transformed_ground_atoms;
    std::unordered_map<Literal, Literal> m_transformed_literals;
    std::unordered_map<GroundLiteral, GroundLiteral> m_transformed_ground_literals;
    std::unordered_map<NumericFluent, NumericFluent> m_transformed_numeric_fluents;
    std::unordered_map<Effect, Effect> m_transformed_effects;
    std::unordered_map<FunctionExpression, FunctionExpression> m_transformed_function_expressions;
    std::unordered_map<FunctionSkeleton, FunctionSkeleton> m_transformed_function_skeletons;
    std::unordered_map<Function, Function> m_transformed_functions;
    std::unordered_map<Action, Action> m_transformed_actions;
    std::unordered_map<Axiom, Axiom> m_transformed_axioms;
    std::unordered_map<Domain, Domain> m_transformed_domains;
    std::unordered_map<OptimizationMetric, OptimizationMetric> m_transformed_optimization_metrics;
    std::unordered_map<Problem, Problem> m_transformed_problems;

    explicit BaseTransformer(PDDLFactories& pddl_factories) : m_pddl_factories(pddl_factories) {}

protected:
    /* Implement ITranslator interface */
    friend class ITransformer<BaseTransformer<Derived>>;

    /// @brief Collect information.
    ///        Default implementation recursively calls prepare.
    void prepare_base(const RequirementsImpl& requirements) { self().prepare_impl(requirements); }
    void prepare_base(const ObjectImpl& object) { self().prepare_impl(object); }
    void prepare_base(const VariableImpl& variable) { self().prepare_impl(variable); }
    void prepare_base(const TermObjectImpl& term) { self().prepare_impl(term); }
    void prepare_base(const TermVariableImpl& term) { self().prepare_impl(term); }
    void prepare_base(const TermImpl& term) { self().prepare_impl(term); }
    void prepare_base(const ParameterImpl& parameter) { self().prepare_impl(parameter); }
    void prepare_base(const PredicateImpl& predicate) { self().prepare_impl(predicate); }
    void prepare_base(const AtomImpl& atom) { self().prepare_impl(atom); }
    void prepare_base(const GroundAtomImpl& atom) { self().prepare_impl(atom); }
    void prepare_base(const LiteralImpl& literal) { self().prepare_impl(literal); }
    void prepare_base(const NumericFluentImpl& numeric_fluent) { self().prepare_impl(numeric_fluent); }
    void prepare_base(const GroundLiteralImpl& literal) { self().prepare_impl(literal); }
    void prepare_base(const EffectImpl& effect) { self().prepare_impl(effect); }
    void prepare_base(const FunctionExpressionNumberImpl& function_expression) { self().prepare_impl(function_expression); }
    void prepare_base(const FunctionExpressionBinaryOperatorImpl& function_expression) { self().prepare_impl(function_expression); }
    void prepare_base(const FunctionExpressionMultiOperatorImpl& function_expression) { self().prepare_impl(function_expression); }
    void prepare_base(const FunctionExpressionMinusImpl& function_expression) { this->prepare(*function_expression.get_function_expression()); }
    void prepare_base(const FunctionExpressionFunctionImpl& function_expression) { this->prepare(*function_expression.get_function()); }
    void prepare_base(const FunctionExpressionImpl& function_expression) { self().prepare_impl(function_expression); }
    void prepare_base(const FunctionSkeletonImpl& function_skeleton) { self().prepare_impl(function_skeleton); }
    void prepare_base(const FunctionImpl& function) { self().prepare_impl(function); }
    void prepare_base(const ActionImpl& action) { self().prepare_impl(action); }
    void prepare_base(const AxiomImpl& axiom) { self().prepare_impl(axiom); }
    void prepare_base(const DomainImpl& domain) { self().prepare_impl(domain); }
    void prepare_base(const OptimizationMetricImpl& metric) { self().prepare_impl(metric); }
    void prepare_base(const ProblemImpl& problem) { self().prepare_impl(problem); }

    void prepare_impl(const RequirementsImpl& requirements) {}
    void prepare_impl(const ObjectImpl& object) {}
    void prepare_impl(const VariableImpl& variable) {}
    void prepare_impl(const TermObjectImpl& term) { this->prepare(*term.get_object()); }
    void prepare_impl(const TermVariableImpl& term) { this->prepare(*term.get_variable()); }
    void prepare_impl(const TermImpl& term)
    {
        std::visit([this](auto&& arg) { return this->prepare(arg); }, term);
    }
    void prepare_impl(const ParameterImpl& parameter) { this->prepare(*parameter.get_variable()); }
    void prepare_impl(const PredicateImpl& predicate) { this->prepare(predicate.get_parameters()); }
    void prepare_impl(const AtomImpl& atom)
    {
        this->prepare(*atom.get_predicate());
        this->prepare(atom.get_terms());
    }
    void prepare_impl(const GroundAtomImpl& atom)
    {
        this->prepare(*atom.get_predicate());
        this->prepare(atom.get_objects());
    }
    void prepare_impl(const LiteralImpl& literal) { this->prepare(*literal.get_atom()); }
    void prepare_impl(const NumericFluentImpl& numeric_fluent) { this->prepare(*numeric_fluent.get_function()); }
    void prepare_impl(const GroundLiteralImpl& literal) { this->prepare(*literal.get_atom()); }
    void prepare_impl(const EffectImpl& effect)
    {
        this->prepare(effect.get_parameters());
        this->prepare(effect.get_conditions());
        this->prepare(*effect.get_effect());
    }
    void prepare_impl(const FunctionExpressionNumberImpl& function_expression) {}
    void prepare_impl(const FunctionExpressionBinaryOperatorImpl& function_expression)
    {
        this->prepare(*function_expression.get_left_function_expression());
        this->prepare(*function_expression.get_right_function_expression());
    }
    void prepare_impl(const FunctionExpressionMultiOperatorImpl& function_expression) { this->prepare(function_expression.get_function_expressions()); }
    void prepare_impl(const FunctionExpressionMinusImpl& function_expression) { this->prepare(*function_expression.get_function_expression()); }
    void prepare_impl(const FunctionExpressionFunctionImpl& function_expression) { this->prepare(*function_expression.get_function()); }
    void prepare_impl(const FunctionExpressionImpl& function_expression)
    {
        std::visit([this](auto&& arg) { return this->prepare(arg); }, function_expression);
    }
    void prepare_impl(const FunctionSkeletonImpl& function_skeleton) { this->prepare(function_skeleton.get_parameters()); }
    void prepare_impl(const FunctionImpl& function)
    {
        this->prepare(*function.get_function_skeleton());
        this->prepare(function.get_terms());
    }
    void prepare_impl(const ActionImpl& action)
    {
        this->prepare(action.get_parameters());
        this->prepare(action.get_conditions());
        this->prepare(action.get_effects());
    }
    void prepare_impl(const AxiomImpl& axiom)
    {
        this->prepare(axiom.get_parameters());
        this->prepare(axiom.get_conditions());
        this->prepare(*axiom.get_literal());
    }
    void prepare_impl(const DomainImpl& domain)
    {
        this->prepare(*domain.get_requirements());
        this->prepare(domain.get_constants());
        this->prepare(domain.get_predicates());
        this->prepare(domain.get_derived_predicates());
        this->prepare(domain.get_functions());
        this->prepare(domain.get_actions());
        this->prepare(domain.get_axioms());
    }
    void prepare_impl(const OptimizationMetricImpl& metric) { this->prepare(*metric.get_function_expression()); }
    void prepare_impl(const ProblemImpl& problem)
    {
        this->prepare(*problem.get_domain());
        this->prepare(*problem.get_requirements());
        this->prepare(problem.get_objects());
        this->prepare(problem.get_derived_predicates());
        this->prepare(problem.get_initial_literals());
        this->prepare(problem.get_numeric_fluents());
        this->prepare(problem.get_goal_condition());
        if (problem.get_optimization_metric().has_value())
        {
            this->prepare(*problem.get_optimization_metric().value());
        }
        this->prepare(problem.get_axioms());
    }

    /// @brief Apply problem translation.
    ///        Default behavior reparses it into the pddl_factories.
    Requirements transform_base(const RequirementsImpl& requirements)
    {
        return cached_transformd_impl(requirements, m_transformed_requirements, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Object transform_base(const ObjectImpl& object)
    {
        return cached_transformd_impl(object, m_transformed_objects, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Variable transform_base(const VariableImpl& variable)
    {
        return cached_transformd_impl(variable, m_transformed_variables, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Term transform_base(const TermObjectImpl& term) { return self().transform_impl(term); }
    Term transform_base(const TermVariableImpl& term) { return self().transform_impl(term); }
    Term transform_base(const TermImpl& term)
    {
        return cached_transformd_impl(term, m_transformed_terms, [this, &term](const auto& arg) { return this->self().transform_impl(term); });
    }
    Parameter transform_base(const ParameterImpl& parameter)
    {
        return cached_transformd_impl(parameter, m_transformed_parameters, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Predicate transform_base(const PredicateImpl& predicate)
    {
        return cached_transformd_impl(predicate, m_transformed_predicates, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Atom transform_base(const AtomImpl& atom)
    {
        return cached_transformd_impl(atom, m_transformed_atoms, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    GroundAtom transform_base(const GroundAtomImpl& atom)
    {
        return cached_transformd_impl(atom, m_transformed_ground_atoms, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Literal transform_base(const LiteralImpl& literal)
    {
        return cached_transformd_impl(literal, m_transformed_literals, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    GroundLiteral transform_base(const GroundLiteralImpl& literal)
    {
        return cached_transformd_impl(literal, m_transformed_ground_literals, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    NumericFluent transform_base(const NumericFluentImpl& numeric_fluent)
    {
        return cached_transformd_impl(numeric_fluent, m_transformed_numeric_fluents, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Effect transform_base(const EffectImpl& effect)
    {
        return cached_transformd_impl(effect, m_transformed_effects, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    FunctionExpression transform_base(const FunctionExpressionNumberImpl& function_expression) { return self().transform_impl(function_expression); }
    FunctionExpression transform_base(const FunctionExpressionBinaryOperatorImpl& function_expression) { return self().transform_impl(function_expression); }
    FunctionExpression transform_base(const FunctionExpressionMultiOperatorImpl& function_expression) { return self().transform_impl(function_expression); }
    FunctionExpression transform_base(const FunctionExpressionMinusImpl& function_expression) { return self().transform_impl(function_expression); }
    FunctionExpression transform_base(const FunctionExpressionFunctionImpl& function_expression) { return self().transform_impl(function_expression); }
    FunctionExpression transform_base(const FunctionExpressionImpl& function_expression)
    {
        return cached_transformd_impl(function_expression,
                                      m_transformed_function_expressions,
                                      [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    FunctionSkeleton transform_base(const FunctionSkeletonImpl& function_skeleton)
    {
        return cached_transformd_impl(function_skeleton,
                                      m_transformed_function_skeletons,
                                      [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Function transform_base(const FunctionImpl& function)
    {
        return cached_transformd_impl(function, m_transformed_functions, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Action transform_base(const ActionImpl& action)
    {
        return cached_transformd_impl(action, m_transformed_actions, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Axiom transform_base(const AxiomImpl& axiom)
    {
        return cached_transformd_impl(axiom, m_transformed_axioms, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Domain transform_base(const DomainImpl& domain)
    {
        return cached_transformd_impl(domain, m_transformed_domains, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    OptimizationMetric transform_base(const OptimizationMetricImpl& metric)
    {
        return cached_transformd_impl(metric, m_transformed_optimization_metrics, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }
    Problem transform_base(const ProblemImpl& problem)
    {
        return cached_transformd_impl(problem, m_transformed_problems, [this](const auto& arg) { return this->self().transform_impl(arg); });
    }

    /// @brief Retrieve or create cache entry of translation to avoid recomputations.
    template<typename Impl, typename TranslateFunc>
    auto cached_transformd_impl(const Impl& impl, std::unordered_map<const Impl*, const Impl*>& cache, const TranslateFunc& transformFunc)
    {
        // Access from cache
        auto it = cache.find(&impl);
        if (it != cache.end())
        {
            return it->second;
        }

        // Translate
        auto transformd = transformFunc(impl);

        // Insert into cache
        cache.emplace(&impl, transformd);

        return transformd;
    }
    Requirements transform_impl(const RequirementsImpl& requirements)
    {
        return this->m_pddl_factories.get_or_create_requirements(requirements.get_requirements());
    }
    Object transform_impl(const ObjectImpl& object) { return this->m_pddl_factories.get_or_create_object(object.get_name()); }
    Variable transform_impl(const VariableImpl& variable)
    {
        return this->m_pddl_factories.get_or_create_variable(variable.get_name(), variable.get_parameter_index());
    }
    Term transform_impl(const TermObjectImpl& term) { return this->m_pddl_factories.get_or_create_term_object(this->transform(*term.get_object())); }
    Term transform_impl(const TermVariableImpl& term) { return this->m_pddl_factories.get_or_create_term_variable(this->transform(*term.get_variable())); }
    Term transform_impl(const TermImpl& term)
    {
        return std::visit([this](auto&& arg) { return this->transform(arg); }, term);
    }
    Parameter transform_impl(const ParameterImpl& parameter)
    {
        return this->m_pddl_factories.get_or_create_parameter(this->transform(*parameter.get_variable()));
    }
    Predicate transform_impl(const PredicateImpl& predicate)
    {
        return this->m_pddl_factories.get_or_create_predicate(predicate.get_name(), this->transform(predicate.get_parameters()));
    }
    Atom transform_impl(const AtomImpl& atom)
    {
        return this->m_pddl_factories.get_or_create_atom(this->transform(*atom.get_predicate()), this->transform(atom.get_terms()));
    }
    GroundAtom transform_impl(const GroundAtomImpl& atom)
    {
        return this->m_pddl_factories.get_or_create_ground_atom(this->transform(*atom.get_predicate()), this->transform(atom.get_objects()));
    }
    Literal transform_impl(const LiteralImpl& literal)
    {
        return this->m_pddl_factories.get_or_create_literal(literal.is_negated(), this->transform(*literal.get_atom()));
    }
    GroundLiteral transform_impl(const GroundLiteralImpl& literal)
    {
        return this->m_pddl_factories.get_or_create_ground_literal(literal.is_negated(), this->transform(*literal.get_atom()));
    }
    NumericFluent transform_impl(const NumericFluentImpl& numeric_fluent)
    {
        return this->m_pddl_factories.get_or_create_numeric_fluent(this->transform(*numeric_fluent.get_function()), numeric_fluent.get_number());
    }
    Effect transform_impl(const EffectImpl& effect)
    {
        return this->m_pddl_factories.get_or_create_simple_effect(this->transform(effect.get_parameters()),
                                                                  this->transform(effect.get_conditions()),
                                                                  this->transform(effect.get_static_conditions()),
                                                                  this->transform(effect.get_static_conditions()),
                                                                  this->transform(*effect.get_effect()));
    }
    FunctionExpression transform_impl(const FunctionExpressionNumberImpl& function_expression)
    {
        return this->m_pddl_factories.get_or_create_function_expression_number(function_expression.get_number());
    }
    FunctionExpression transform_impl(const FunctionExpressionBinaryOperatorImpl& function_expression)
    {
        return this->m_pddl_factories.get_or_create_function_expression_binary_operator(function_expression.get_binary_operator(),
                                                                                        this->transform(*function_expression.get_left_function_expression()),
                                                                                        this->transform(*function_expression.get_right_function_expression()));
    }
    FunctionExpression transform_impl(const FunctionExpressionMultiOperatorImpl& function_expression)
    {
        return this->m_pddl_factories.get_or_create_function_expression_multi_operator(function_expression.get_multi_operator(),
                                                                                       this->transform(function_expression.get_function_expressions()));
    }
    FunctionExpression transform_impl(const FunctionExpressionMinusImpl& function_expression)
    {
        return this->m_pddl_factories.get_or_create_function_expression_minus(this->transform(*function_expression.get_function_expression()));
    }
    FunctionExpression transform_impl(const FunctionExpressionFunctionImpl& function_expression)
    {
        return this->m_pddl_factories.get_or_create_function_expression_function(this->transform(*function_expression.get_function()));
    }
    FunctionExpression transform_impl(const FunctionExpressionImpl& function_expression)
    {
        return std::visit([this](auto&& arg) { return this->transform(arg); }, function_expression);
    }
    FunctionSkeleton transform_impl(const FunctionSkeletonImpl& function_skeleton)
    {
        return this->m_pddl_factories.get_or_create_function_skeleton(function_skeleton.get_name(), this->transform(function_skeleton.get_parameters()));
    }
    Function transform_impl(const FunctionImpl& function)
    {
        return this->m_pddl_factories.get_or_create_function(this->transform(*function.get_function_skeleton()), this->transform(function.get_terms()));
    }
    Action transform_impl(const ActionImpl& action)
    {
        return this->m_pddl_factories.get_or_create_action(action.get_name(),
                                                           this->transform(action.get_parameters()),
                                                           this->transform(action.get_conditions()),
                                                           this->transform(action.get_static_conditions()),
                                                           this->transform(action.get_fluent_conditions()),
                                                           this->transform(action.get_effects()));
    }
    Axiom transform_impl(const AxiomImpl& axiom)
    {
        return this->m_pddl_factories.get_or_create_axiom(this->transform(axiom.get_parameters()),
                                                          this->transform(*axiom.get_literal()),
                                                          this->transform(axiom.get_conditions()),
                                                          this->transform(axiom.get_static_conditions()),
                                                          this->transform(axiom.get_fluent_conditions()));
    }
    Domain transform_impl(const DomainImpl& domain)
    {
        return this->m_pddl_factories.get_or_create_domain(domain.get_name(),
                                                           this->transform(*domain.get_requirements()),
                                                           this->transform(domain.get_constants()),
                                                           this->transform(domain.get_predicates()),
                                                           this->transform(domain.get_static_predicates()),
                                                           this->transform(domain.get_fluent_predicates()),
                                                           this->transform(domain.get_derived_predicates()),
                                                           this->transform(domain.get_functions()),
                                                           this->transform(domain.get_actions()),
                                                           this->transform(domain.get_axioms()));
    }
    OptimizationMetric transform_impl(const OptimizationMetricImpl& metric)
    {
        return this->m_pddl_factories.get_or_create_optimization_metric(metric.get_optimization_metric(), this->transform(*metric.get_function_expression()));
    }

    Problem transform_impl(const ProblemImpl& problem)
    {
        return this->m_pddl_factories.get_or_create_problem(
            this->transform(*problem.get_domain()),
            problem.get_name(),
            this->transform(*problem.get_requirements()),
            this->transform(problem.get_objects()),
            this->transform(problem.get_derived_predicates()),
            this->transform(problem.get_initial_literals()),
            this->transform(problem.get_numeric_fluents()),
            this->transform(problem.get_goal_condition()),
            (problem.get_optimization_metric().has_value() ? std::optional<OptimizationMetric>(this->transform(*problem.get_optimization_metric().value())) :
                                                             std::nullopt),
            this->transform(problem.get_axioms()));
    }

    /// @brief Recursively apply preparation followed by transformation.
    ///        Default behavior runs prepare and transform and returns its results.
    Problem run_base(const ProblemImpl& problem) { return self().run_impl(problem); }

    Problem run_impl(const ProblemImpl& problem)
    {
        self().prepare(problem);
        return self().transform(problem);
    }
};
}

#endif
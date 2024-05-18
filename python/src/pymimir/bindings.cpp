#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // Necessary for automatic conversion of e.g. std::vectors

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include "mimir/mimir.hpp"

using namespace mimir;
namespace py = pybind11;

/**
 * We cannot expose the variant types directly because they are not default constructible.
 */

struct TermVariant
{
    Term term;
    explicit TermVariant(const Term& t) : term(t) {}
};

struct FunctionExpressionVariant
{
    FunctionExpression function_expression;
    explicit FunctionExpressionVariant(const FunctionExpression& e) : function_expression(e) {}
};

struct GroundFunctionExpressionVariant
{
    GroundFunctionExpression function_expression;
    explicit GroundFunctionExpressionVariant(const GroundFunctionExpression& e) : function_expression(e) {}
};

std::vector<TermVariant> to_term_variant_list(const TermList& terms)
{
    auto result = std::vector<TermVariant> {};
    result.reserve(terms.size());
    for (const auto& term : terms)
    {
        result.push_back(TermVariant(term));
    }
    return result;
}

std::vector<FunctionExpressionVariant> to_function_expression_variant_list(const FunctionExpressionList& function_expressions)
{
    auto result = std::vector<FunctionExpressionVariant> {};
    result.reserve(function_expressions.size());
    for (const auto& function_expression : function_expressions)
    {
        result.push_back(FunctionExpressionVariant(function_expression));
    }
    return result;
}

std::vector<GroundFunctionExpressionVariant> to_ground_function_expression_variant_list(const GroundFunctionExpressionList& ground_function_expressions)
{
    auto result = std::vector<GroundFunctionExpressionVariant> {};
    result.reserve(ground_function_expressions.size());
    for (const auto& function_expression : ground_function_expressions)
    {
        result.push_back(GroundFunctionExpressionVariant(function_expression));
    }
    return result;
}

struct CastVisitor
{
    template<typename T>
    py::object operator()(const T& element) const
    {
        return py::cast(element);
    }
};

/**
 * Bindings
 */
void init_pymimir(py::module_& m)
{
    /**
     * Formalism
     */

    py::enum_<loki::RequirementEnum>(m, "RequirementEnum")
        .value("STRIPS", loki::RequirementEnum::STRIPS)
        .value("TYPING", loki::RequirementEnum::TYPING)
        .value("NEGATIVE_PRECONDITIONS", loki::RequirementEnum::NEGATIVE_PRECONDITIONS)
        .value("DISJUNCTIVE_PRECONDITIONS", loki::RequirementEnum::DISJUNCTIVE_PRECONDITIONS)
        .value("EQUALITY", loki::RequirementEnum::EQUALITY)
        .value("EXISTENTIAL_PRECONDITIONS", loki::RequirementEnum::EXISTENTIAL_PRECONDITIONS)
        .value("UNIVERSAL_PRECONDITIONS", loki::RequirementEnum::UNIVERSAL_PRECONDITIONS)
        .value("QUANTIFIED_PRECONDITIONS", loki::RequirementEnum::QUANTIFIED_PRECONDITIONS)
        .value("CONDITIONAL_EFFECTS", loki::RequirementEnum::CONDITIONAL_EFFECTS)
        .value("FLUENTS", loki::RequirementEnum::FLUENTS)
        .value("OBJECT_FLUENTS", loki::RequirementEnum::OBJECT_FLUENTS)
        .value("NUMERIC_FLUENTS", loki::RequirementEnum::NUMERIC_FLUENTS)
        .value("ADL", loki::RequirementEnum::ADL)
        .value("DURATIVE_ACTIONS", loki::RequirementEnum::DURATIVE_ACTIONS)
        .value("DERIVED_PREDICATES", loki::RequirementEnum::DERIVED_PREDICATES)
        .value("TIMED_INITIAL_LITERALS", loki::RequirementEnum::TIMED_INITIAL_LITERALS)
        .value("PREFERENCES", loki::RequirementEnum::PREFERENCES)
        .value("CONSTRAINTS", loki::RequirementEnum::CONSTRAINTS)
        .value("ACTION_COSTS", loki::RequirementEnum::ACTION_COSTS)
        .export_values();

    py::enum_<loki::AssignOperatorEnum>(m, "AssignOperatorEnum")
        .value("ASSIGN", loki::AssignOperatorEnum::ASSIGN)
        .value("SCALE_UP", loki::AssignOperatorEnum::SCALE_UP)
        .value("SCALE_DOWN", loki::AssignOperatorEnum::SCALE_DOWN)
        .value("INCREASE", loki::AssignOperatorEnum::INCREASE)
        .value("DECREASE", loki::AssignOperatorEnum::DECREASE)
        .export_values();

    py::enum_<loki::BinaryOperatorEnum>(m, "BinaryOperatorEnum")
        .value("MUL", loki::BinaryOperatorEnum::MUL)
        .value("PLUS", loki::BinaryOperatorEnum::PLUS)
        .value("MINUS", loki::BinaryOperatorEnum::MINUS)
        .value("DIV", loki::BinaryOperatorEnum::DIV)
        .export_values();

    py::enum_<loki::MultiOperatorEnum>(m, "MultiOperatorEnum")
        .value("MUL", loki::MultiOperatorEnum::MUL)
        .value("PLUS", loki::MultiOperatorEnum::PLUS)
        .export_values();

    pybind11::enum_<loki::OptimizationMetricEnum>(m, "OptimizationMetricEnum")
        .value("MINIMIZE", loki::OptimizationMetricEnum::MINIMIZE)
        .value("MAXIMIZE", loki::OptimizationMetricEnum::MAXIMIZE)
        .export_values();

    py::class_<RequirementsImpl>(m, "Requirements")  //
        .def("__str__", py::overload_cast<>(&loki::Base<RequirementsImpl>::str, py::const_))
        .def("get_identifier", &RequirementsImpl::get_identifier)
        .def("get_requirements", &RequirementsImpl::get_requirements, py::return_value_policy::reference);

    py::class_<ObjectImpl>(m, "Object")  //
        .def("__str__", py::overload_cast<>(&loki::Base<ObjectImpl>::str, py::const_))
        .def("get_identifier", &ObjectImpl::get_identifier)
        .def("get_name", &ObjectImpl::get_name, py::return_value_policy::reference);

    py::class_<VariableImpl>(m, "Variable")  //
        .def("__str__", py::overload_cast<>(&loki::Base<VariableImpl>::str, py::const_))
        .def("get_identifier", &VariableImpl::get_identifier)
        .def("get_name", &VariableImpl::get_name, py::return_value_policy::reference);

    py::class_<TermObjectImpl>(m, "TermObject")  //
        .def("__str__", py::overload_cast<>(&loki::Base<TermObjectImpl>::str, py::const_))
        .def("get_identifier", &TermObjectImpl::get_identifier)
        .def("get_object", &TermObjectImpl::get_object, py::return_value_policy::reference);

    py::class_<TermVariableImpl>(m, "TermVariable")  //
        .def("__str__", py::overload_cast<>(&loki::Base<TermVariableImpl>::str, py::const_))
        .def("get_identifier", &TermVariableImpl::get_identifier)
        .def("get_variable", &TermVariableImpl::get_variable, py::return_value_policy::reference);

    py::class_<TermVariant>(m, "Term")  //
        .def("get", [](const TermVariant& arg) -> py::object { return std::visit(CastVisitor(), *arg.term); });

    py::class_<PredicateImpl>(m, "Predicate")  //
        .def("__str__", py::overload_cast<>(&loki::Base<PredicateImpl>::str, py::const_))
        .def("get_identifier", &PredicateImpl::get_identifier)
        .def("get_name", &PredicateImpl::get_name, py::return_value_policy::reference)
        .def("get_parameters", &PredicateImpl::get_parameters, py::return_value_policy::reference);

    py::class_<AtomImpl>(m, "Atom")  //
        .def("__str__", py::overload_cast<>(&loki::Base<AtomImpl>::str, py::const_))
        .def("get_identifier", &AtomImpl::get_identifier)
        .def("get_predicate", &AtomImpl::get_predicate, py::return_value_policy::reference)
        .def("get_terms", [](const AtomImpl& atom) { return to_term_variant_list(atom.get_terms()); });

    py::class_<FunctionSkeletonImpl>(m, "FunctionSkeleton")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionSkeletonImpl>::str, py::const_))
        .def("get_identifier", &FunctionSkeletonImpl::get_identifier)
        .def("get_name", &FunctionSkeletonImpl::get_name, py::return_value_policy::reference)
        .def("get_parameters", &FunctionSkeletonImpl::get_parameters, py::return_value_policy::reference);

    py::class_<FunctionImpl>(m, "Function")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionImpl>::str, py::const_))
        .def("get_identifier", &FunctionImpl::get_identifier)
        .def("get_function_skeleton", &FunctionImpl::get_function_skeleton, py::return_value_policy::reference)
        .def("get_terms", [](const FunctionImpl& function) { return to_term_variant_list(function.get_terms()); });

    py::class_<GroundFunctionImpl>(m, "GroundFunction")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundFunctionImpl>::str, py::const_))
        .def("get_identifier", &GroundFunctionImpl::get_identifier)
        .def("get_function_skeleton", &GroundFunctionImpl::get_function_skeleton, py::return_value_policy::reference)
        .def("get_objects", &GroundFunctionImpl::get_objects, py::return_value_policy::reference);

    py::class_<GroundAtomImpl>(m, "GroundAtom")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundAtomImpl>::str, py::const_))
        .def("get_identifier", &GroundAtomImpl::get_identifier)
        .def("get_arity", &GroundAtomImpl::get_arity)
        .def("get_predicate", &GroundAtomImpl::get_predicate, py::return_value_policy::reference)
        .def("get_objects", &GroundAtomImpl::get_objects, py::return_value_policy::reference);

    py::class_<GroundLiteralImpl>(m, "GroundLiteral")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundLiteralImpl>::str, py::const_))
        .def("get_identifier", &GroundLiteralImpl::get_identifier)
        .def("get_atom", &GroundLiteralImpl::get_atom, py::return_value_policy::reference)
        .def("is_negated", &GroundLiteralImpl::is_negated);

    // We get typing.Literal in the generated stubs when using Literal as name
    py::class_<LiteralImpl>(m, "_Literal")  //
        .def("__str__", py::overload_cast<>(&loki::Base<LiteralImpl>::str, py::const_))
        .def("get_identifier", &LiteralImpl::get_identifier)
        .def("get_atom", &LiteralImpl::get_atom, py::return_value_policy::reference)
        .def("is_negated", &LiteralImpl::is_negated);

    py::class_<NumericFluentImpl>(m, "NumericFluent")  //
        .def("__str__", py::overload_cast<>(&loki::Base<NumericFluentImpl>::str, py::const_))
        .def("get_identifier", &NumericFluentImpl::get_identifier)
        .def("get_function", &NumericFluentImpl::get_function, py::return_value_policy::reference)
        .def("get_number", &NumericFluentImpl::get_number);

    py::class_<EffectSimpleImpl>(m, "SimpleEffect")  //
        .def("__str__", py::overload_cast<>(&loki::Base<EffectSimpleImpl>::str, py::const_))
        .def("get_identifier", &EffectSimpleImpl::get_identifier)
        .def("get_effect", &EffectSimpleImpl::get_effect, py::return_value_policy::reference);

    py::class_<EffectConditionalImpl>(m, "ConditionalEffect")  //
        .def("__str__", py::overload_cast<>(&loki::Base<EffectConditionalImpl>::str, py::const_))
        .def("get_identifier", &EffectConditionalImpl::get_identifier)
        .def("get_conditions", &EffectConditionalImpl::get_conditions, py::return_value_policy::reference)
        .def("get_static_conditions", &EffectConditionalImpl::get_static_conditions, py::return_value_policy::reference)
        .def("get_fluent_conditions", &EffectConditionalImpl::get_fluent_conditions, py::return_value_policy::reference)
        .def("get_effect", &EffectConditionalImpl::get_effect, py::return_value_policy::reference);

    py::class_<FunctionExpressionVariant>(m, "FunctionExpression")  //
        .def("get", [](const FunctionExpressionVariant& arg) -> py::object { return std::visit(CastVisitor(), *arg.function_expression); });
    ;

    py::class_<EffectUniversalImpl>(m, "UniversalEffect")  //
        .def("__str__", py::overload_cast<>(&loki::Base<EffectUniversalImpl>::str, py::const_))
        .def("get_identifier", &EffectUniversalImpl::get_identifier)
        .def("get_parameters", &EffectUniversalImpl::get_parameters, py::return_value_policy::reference)
        .def("get_conditions", &EffectUniversalImpl::get_conditions, py::return_value_policy::reference)
        .def("get_static_conditions", &EffectUniversalImpl::get_static_conditions, py::return_value_policy::reference)
        .def("get_fluent_conditions", &EffectUniversalImpl::get_fluent_conditions, py::return_value_policy::reference)
        .def("get_effect", &EffectUniversalImpl::get_effect, py::return_value_policy::reference);

    py::class_<FunctionExpressionNumberImpl>(m, "FunctionExpressionNumber")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionExpressionNumberImpl>::str, py::const_))
        .def("get_identifier", &FunctionExpressionNumberImpl::get_identifier)
        .def("get_number", &FunctionExpressionNumberImpl::get_number);

    py::class_<FunctionExpressionBinaryOperatorImpl>(m, "FunctionExpressionBinaryOperator")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionExpressionBinaryOperatorImpl>::str, py::const_))
        .def("get_identifier", &FunctionExpressionBinaryOperatorImpl::get_identifier)
        .def("get_binary_operator", &FunctionExpressionBinaryOperatorImpl::get_binary_operator)
        .def("get_left_function_expression",
             [](const FunctionExpressionBinaryOperatorImpl& function_expression)
             { return FunctionExpressionVariant(function_expression.get_left_function_expression()); })
        .def("get_right_function_expression",
             [](const FunctionExpressionBinaryOperatorImpl& function_expression)
             { return FunctionExpressionVariant(function_expression.get_right_function_expression()); });

    py::class_<FunctionExpressionMultiOperatorImpl>(m, "FunctionExpressionMultiOperator")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionExpressionMultiOperatorImpl>::str, py::const_))
        .def("get_identifier", &FunctionExpressionMultiOperatorImpl::get_identifier)
        .def("get_multi_operator", &FunctionExpressionMultiOperatorImpl::get_multi_operator)
        .def("get_function_expressions",
             [](const FunctionExpressionMultiOperatorImpl& function_expression)
             { return to_function_expression_variant_list(function_expression.get_function_expressions()); });

    py::class_<FunctionExpressionMinusImpl>(m, "FunctionExpressionMinus")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionExpressionMinusImpl>::str, py::const_))
        .def("get_identifier", &FunctionExpressionMinusImpl::get_identifier)
        .def("get_function_expression",
             [](const FunctionExpressionMinusImpl& function_expression) { return FunctionExpressionVariant(function_expression.get_function_expression()); });
    ;

    py::class_<FunctionExpressionFunctionImpl>(m, "FunctionExpressionFunction")  //
        .def("__str__", py::overload_cast<>(&loki::Base<FunctionExpressionFunctionImpl>::str, py::const_))
        .def("get_identifier", &FunctionExpressionFunctionImpl::get_identifier)
        .def("get_function", &FunctionExpressionFunctionImpl::get_function);

    py::class_<GroundFunctionExpressionVariant>(m, "GroundFunctionExpression")  //
        .def("get", [](const GroundFunctionExpressionVariant& arg) -> py::object { return std::visit(CastVisitor(), *arg.function_expression); });
    ;

    py::class_<GroundFunctionExpressionNumberImpl>(m, "GroundFunctionExpressionNumber")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundFunctionExpressionNumberImpl>::str, py::const_))
        .def("get_identifier", &GroundFunctionExpressionNumberImpl::get_identifier)
        .def("get_number", &GroundFunctionExpressionNumberImpl::get_number);

    py::class_<GroundFunctionExpressionBinaryOperatorImpl>(m, "GroundFunctionExpressionBinaryOperator")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundFunctionExpressionBinaryOperatorImpl>::str, py::const_))
        .def("get_identifier", &GroundFunctionExpressionBinaryOperatorImpl::get_identifier)
        .def("get_binary_operator", &GroundFunctionExpressionBinaryOperatorImpl::get_binary_operator)
        .def("get_left_function_expression",
             [](const GroundFunctionExpressionBinaryOperatorImpl& function_expression)
             { return GroundFunctionExpressionVariant(function_expression.get_left_function_expression()); })
        .def("get_right_function_expression",
             [](const GroundFunctionExpressionBinaryOperatorImpl& function_expression)
             { return GroundFunctionExpressionVariant(function_expression.get_right_function_expression()); });

    py::class_<GroundFunctionExpressionMultiOperatorImpl>(m, "GroundFunctionExpressionMultiOperator")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundFunctionExpressionMultiOperatorImpl>::str, py::const_))
        .def("get_identifier", &GroundFunctionExpressionMultiOperatorImpl::get_identifier)
        .def("get_multi_operator", &GroundFunctionExpressionMultiOperatorImpl::get_multi_operator)
        .def("get_function_expressions",
             [](const GroundFunctionExpressionMultiOperatorImpl& function_expression)
             { return to_ground_function_expression_variant_list(function_expression.get_function_expressions()); });

    py::class_<GroundFunctionExpressionMinusImpl>(m, "GroundFunctionExpressionMinus")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundFunctionExpressionMinusImpl>::str, py::const_))
        .def("get_identifier", &GroundFunctionExpressionMinusImpl::get_identifier)
        .def("get_function_expression",
             [](const GroundFunctionExpressionMinusImpl& function_expression)
             { return GroundFunctionExpressionVariant(function_expression.get_function_expression()); });
    ;

    py::class_<GroundFunctionExpressionFunctionImpl>(m, "GroundFunctionExpressionFunction")  //
        .def("__str__", py::overload_cast<>(&loki::Base<GroundFunctionExpressionFunctionImpl>::str, py::const_))
        .def("get_identifier", &GroundFunctionExpressionFunctionImpl::get_identifier)
        .def("get_function", &GroundFunctionExpressionFunctionImpl::get_function);

    py::class_<OptimizationMetricImpl>(m, "OptimizationMetric")  //
        .def("__str__", py::overload_cast<>(&loki::Base<OptimizationMetricImpl>::str, py::const_))
        .def("get_identifier", &OptimizationMetricImpl::get_identifier)
        .def("get_function_expression", [](const OptimizationMetricImpl& metric) { return GroundFunctionExpressionVariant(metric.get_function_expression()); })
        .def("get_optimization_metric", &OptimizationMetricImpl::get_optimization_metric, py::return_value_policy::reference);

    py::class_<ActionImpl>(m, "Action")  //
        .def("__str__", py::overload_cast<>(&loki::Base<ActionImpl>::str, py::const_))
        .def("get_identifier", &ActionImpl::get_identifier)
        .def("get_name", &ActionImpl::get_name, py::return_value_policy::reference)
        .def("get_arity", &ActionImpl::get_arity)
        .def("get_parameters", &ActionImpl::get_parameters, py::return_value_policy::reference)
        .def("get_conditions", &ActionImpl::get_conditions, py::return_value_policy::reference)
        .def("get_static_conditions", &ActionImpl::get_static_conditions, py::return_value_policy::reference)
        .def("get_fluent_conditions", &ActionImpl::get_fluent_conditions, py::return_value_policy::reference)
        .def("get_simple_effects", &ActionImpl::get_simple_effects, py::return_value_policy::reference)
        .def("get_conditional_effects", &ActionImpl::get_conditional_effects, py::return_value_policy::reference)
        .def("get_universal_effects", &ActionImpl::get_universal_effects, py::return_value_policy::reference);

    py::class_<AxiomImpl>(m, "Axiom")  //
        .def("__str__", py::overload_cast<>(&loki::Base<AxiomImpl>::str, py::const_))
        .def("get_identifier", &AxiomImpl::get_identifier)
        .def("get_literal", &AxiomImpl::get_literal, py::return_value_policy::reference)
        .def("get_conditions", &AxiomImpl::get_conditions, py::return_value_policy::reference)
        .def("get_static_conditions", &AxiomImpl::get_static_conditions, py::return_value_policy::reference)
        .def("get_fluent_conditions", &AxiomImpl::get_fluent_conditions, py::return_value_policy::reference);

    py::class_<DomainImpl>(m, "Domain")  //
        .def("__str__", py::overload_cast<>(&loki::Base<DomainImpl>::str, py::const_))
        .def("get_identifier", &DomainImpl::get_identifier)
        .def("get_name", &DomainImpl::get_name, py::return_value_policy::reference)
        .def("get_constants", &DomainImpl::get_constants, py::return_value_policy::reference)
        .def("get_predicates", &DomainImpl::get_predicates, py::return_value_policy::reference)
        .def("get_fluent_predicates", &DomainImpl::get_fluent_predicates, py::return_value_policy::reference)
        .def("get_static_predicates", &DomainImpl::get_static_predicates, py::return_value_policy::reference)
        .def("get_functions", &DomainImpl::get_functions, py::return_value_policy::reference)
        .def("get_actions", &DomainImpl::get_actions, py::return_value_policy::reference)
        .def("get_requirements", &DomainImpl::get_requirements, py::return_value_policy::reference);

    py::class_<ProblemImpl>(m, "Problem")  //
        .def("__str__", py::overload_cast<>(&loki::Base<ProblemImpl>::str, py::const_))
        .def("get_identifier", &ProblemImpl::get_identifier)
        .def("get_name", &ProblemImpl::get_name, py::return_value_policy::reference)
        .def("get_domain", &ProblemImpl::get_domain, py::return_value_policy::reference)
        .def("get_requirements", &ProblemImpl::get_requirements, py::return_value_policy::reference)
        .def("get_objects", &ProblemImpl::get_objects, py::return_value_policy::reference)
        .def("get_initial_literals", &ProblemImpl::get_initial_literals, py::return_value_policy::reference)
        .def("get_numeric_fluents", &ProblemImpl::get_numeric_fluents, py::return_value_policy::reference)
        .def("get_optimization_metric", &ProblemImpl::get_optimization_metric, py::return_value_policy::reference)
        .def("get_goal_condition", &ProblemImpl::get_goal_condition, py::return_value_policy::reference);

    py::class_<PDDLFactories>(m, "PDDLFactories");

    py::class_<PDDLParser>(m, "PDDLParser")  //
        .def(py::init<std::string, std::string>())
        .def("get_domain", &PDDLParser::get_domain, py::return_value_policy::reference)
        .def("get_problem", &PDDLParser::get_problem, py::return_value_policy::reference)
        .def("get_factories", &PDDLParser::get_factories, py::return_value_policy::reference);

    /**
     * Search
     */

    py::enum_<SearchNodeStatus>(m, "SearchNodeStatus")
        .value("NEW", SearchNodeStatus::NEW)
        .value("OPEN", SearchNodeStatus::OPEN)
        .value("CLOSED", SearchNodeStatus::CLOSED)
        .value("DEAD_END", SearchNodeStatus::DEAD_END)
        .export_values();

    py::enum_<SearchStatus>(m, "SearchStatus")
        .value("IN_PROGRESS", SearchStatus::IN_PROGRESS)
        .value("OUT_OF_TIME", SearchStatus::OUT_OF_TIME)
        .value("OUT_OF_MEMORY", SearchStatus::OUT_OF_MEMORY)
        .value("FAILED", SearchStatus::FAILED)
        .value("EXHAUSTED", SearchStatus::EXHAUSTED)
        .value("SOLVED", SearchStatus::SOLVED)
        .export_values();

    /* State */
    py::class_<State>(m, "State")  //
        .def("__hash__", &State::hash)
        .def("__eq__", &State::operator==)
        .def(
            "__iter__",
            [](State& state) { return py::make_iterator(state.begin(), state.end()); },
            py::keep_alive<0, 1>())
        .def("to_string",
             [](State self, PDDLFactories& pddl_factories)
             {
                 std::stringstream ss;
                 ss << std::make_tuple(self, std::cref(pddl_factories));
                 return ss.str();
             })
        .def("get_id", &State::get_id);

    /* Action */
    py::class_<GroundAction>(m, "GroundAction")  //
        .def("__hash__", &GroundAction::hash)
        .def("__eq__", &GroundAction::operator==)
        .def("to_string",
             [](GroundAction self, PDDLFactories& pddl_factories)
             {
                 std::stringstream ss;
                 ss << std::make_tuple(self, std::cref(pddl_factories));
                 return ss.str();
             })
        .def("get_id", &GroundAction::get_id);

    /* AAGs */
    py::class_<IDynamicAAG, std::shared_ptr<IDynamicAAG>>(m, "IAAG")  //
        .def("generate_applicable_actions", &IDynamicAAG::generate_applicable_actions)
        .def("get_action", &IDynamicAAG::get_action)
        .def("get_problem", &IDynamicAAG::get_problem)
        .def("get_pddl_factories", py::overload_cast<>(&IDynamicAAG::get_pddl_factories), py::return_value_policy::reference);
    py::class_<LiftedAAG, IDynamicAAG, std::shared_ptr<LiftedAAG>>(m, "LiftedAAG")  //
        .def(py::init<Problem, PDDLFactories&>());
    py::class_<GroundedAAG, IDynamicAAG, std::shared_ptr<GroundedAAG>>(m, "GroundedAAG")  //
        .def(py::init<Problem, PDDLFactories&>());

    /* SSGs */
    py::class_<IDynamicSSG, std::shared_ptr<IDynamicSSG>>(m, "ISSG")  //
        .def("get_or_create_initial_state", &IDynamicSSG::get_or_create_initial_state)
        .def("get_or_create_state", &IDynamicSSG::get_or_create_state)
        .def("get_or_create_successor_state", &IDynamicSSG::get_or_create_successor_state)
        .def("get_non_extended_state", &IDynamicSSG::get_non_extended_state)
        .def("get_state_count", &IDynamicSSG::get_state_count);
    py::class_<SuccessorStateGenerator, IDynamicSSG, std::shared_ptr<SuccessorStateGenerator>>(m, "SSG")  //
        .def(py::init<std::shared_ptr<IDynamicAAG>>());

    /* Heuristics */
    py::class_<IDynamicHeuristic, std::shared_ptr<IDynamicHeuristic>>(m, "IHeuristic");
    py::class_<BlindHeuristic, IDynamicHeuristic, std::shared_ptr<BlindHeuristic>>(m, "BlindHeuristic").def(py::init<>());

    /* Event Handlers */
    py::class_<IEventHandler, std::shared_ptr<IEventHandler>>(m, "IEventHandler");
    py::class_<MinimalEventHandler, IEventHandler, std::shared_ptr<MinimalEventHandler>>(m, "MinimalEventHandler")  //
        .def(py::init<>());
    py::class_<DebugEventHandler, IEventHandler, std::shared_ptr<DebugEventHandler>>(m, "DebugEventHandler")  //
        .def(py::init<>());

    /* Algorithms */
    py::class_<IAlgorithm, std::shared_ptr<IAlgorithm>>(m, "IAlgorithm")  //
        .def("find_solution",
             [](IAlgorithm& algorithm)
             {
                 auto out_actions = GroundActionList {};
                 auto search_status = algorithm.find_solution(out_actions);
                 return std::make_tuple(search_status, out_actions);
             });
    py::class_<BrFsAlgorithm, IAlgorithm, std::shared_ptr<BrFsAlgorithm>>(m, "BrFsAlgorithm")  //
        .def(py::init<std::shared_ptr<IDynamicAAG>, std::shared_ptr<IEventHandler>>());
    py::class_<AStarAlgorithm, IAlgorithm, std::shared_ptr<AStarAlgorithm>>(m, "AStarAlgorithm")  //
        .def(py::init<std::shared_ptr<IDynamicAAG>, std::shared_ptr<IDynamicHeuristic>, std::shared_ptr<IEventHandler>>());
}
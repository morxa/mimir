from typing import Union, List, MutableSet
from enum import Enum


class RequirementEnum(Enum):
    STRIPS: 'RequirementEnum'
    TYPING: 'RequirementEnum'
    NEGATIVE_PRECONDITIONS: 'RequirementEnum'
    DISJUNCTIVE_PRECONDITIONS: 'RequirementEnum'
    EQUALITY: 'RequirementEnum'
    EXISTENTIAL_PRECONDITIONS: 'RequirementEnum'
    UNIVERSAL_PRECONDITIONS: 'RequirementEnum'
    QUANTIFIED_PRECONDITIONS: 'RequirementEnum'
    CONDITIONAL_EFFECTS: 'RequirementEnum'
    FLUENTS: 'RequirementEnum'
    OBJECT_FLUENTS: 'RequirementEnum'
    NUMERIC_FLUENTS: 'RequirementEnum'
    ADL: 'RequirementEnum'
    DURATIVE_ACTIONS: 'RequirementEnum'
    DERIVED_PREDICATES: 'RequirementEnum'
    TIMED_INITIAL_LITERALS: 'RequirementEnum'
    PREFERENCES: 'RequirementEnum'
    CONSTRAINTS: 'RequirementEnum'
    ACTION_COSTS: 'RequirementEnum'

class AssignOperatorEnum(Enum):
    ASSIGN: 'AssignOperatorEnum'
    SCALE_UP: 'AssignOperatorEnum'
    SCALE_DOWN: 'AssignOperatorEnum'
    INCREASE: 'AssignOperatorEnum'
    DECREASE: 'AssignOperatorEnum'

class BinaryOperatorEnum(Enum):
    MUL: 'BinaryOperatorEnum'
    PLUS: 'BinaryOperatorEnum'
    MINUS: 'BinaryOperatorEnum'
    DIV: 'BinaryOperatorEnum'

class MultiOperatorEnum(Enum):
    MUL: 'MultiOperatorEnum'
    PLUS: 'MultiOperatorEnum'

class OptimizationMetricEnum(Enum):
    MINIMIZE: 'OptimizationMetricEnum'
    MAXIMIZE: 'OptimizationMetricEnum'

class Variable:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...

class Type:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_bases(self) -> List["Type"]: ...

class Object:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_bases(self) -> List[Type]: ...

class Parameter:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_variable(self) -> Variable: ...
    def get_bases(self) -> List[Type]: ...

class Predicate:
    def __str__(self) -> str: ...
    def get_name(self) -> str: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_identifier(self) -> int: ...

class TermObject:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_object(self) -> Object: ...

class TermVariable:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_variable(self) -> Variable: ...

class Term:
    def get(self) -> Union[TermObject, TermVariable]: ...

class Requirements:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_requirements(self) -> MutableSet[RequirementEnum]: ...

class FunctionSkeleton:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_type(self) -> Type: ...

class Function:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_function_skeleton(self) -> FunctionSkeleton: ...
    def get_terms(self) -> List[Term]: ...

class Atom:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_predicate(self) -> Predicate: ...
    def get_terms(self) -> List[Term]: ...

class Literal:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_atom(self) -> Atom: ...
    def is_negated(self) -> bool: ...

class GroundAtom:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_predicate(self) -> Predicate: ...
    def get_terms(self) -> List[Term]: ...

class GroundLiteral:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_atom(self) -> Atom: ...
    def is_negated(self) -> bool: ...

class FunctionExpressionNumber:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_number(self) -> int: ...

class FunctionExpressionBinaryOperator:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_binary_operator(self) -> BinaryOperatorEnum: ...
    def get_left_function_expression(self) -> "FunctionExpression": ...
    def get_right_function_expression(self) -> "FunctionExpression": ...

class FunctionExpressionMultiOperator:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_multi_operator(self) -> MultiOperatorEnum: ...
    def get_function_expressions(self) -> List[FunctionExpression]: ...

class FunctionExpressionMinus:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_function_expression(self) -> List[FunctionExpression]: ...

class FunctionExpressionFunction:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_function(self) -> Function: ...

class FunctionExpression:
    def get(self) -> Union[FunctionExpressionNumber, FunctionExpressionBinaryOperator, FunctionExpressionMultiOperator, FunctionExpressionMinus, FunctionExpressionFunction]: ...

class NumericFluent:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_assign_operator(self) -> AssignOperatorEnum: ...
    def get_function(self) -> Function: ...
    def get_function_expression(self) -> FunctionExpression: ...

class ConditionLiteral:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_literal(self) -> Literal: ...

class ConditionAnd:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_conditions(self) -> List[Condition]: ...

class ConditionOr:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_conditions(self) -> List[Condition]: ...

class ConditionNot:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_condition(self) -> Condition: ...

class ConditionImply:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_condition_left(self) -> Condition: ...
    def get_condition_right(self) -> Condition: ...

class ConditionExists:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_condition(self) -> Condition: ...

class ConditionForall:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_condition(self) -> Condition: ...

class Condition:
    def get(self) -> Union[ConditionLiteral, ConditionAnd, ConditionOr, ConditionNot, ConditionImply, ConditionExists, ConditionForall]: ...

class EffectLiteral:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_literal(self) -> Literal: ...

class EffectAnd:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_effects(self) -> List[Effect]: ...

class EffectNumeric:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_assign_operator(self) -> AssignOperatorEnum: ...
    def get_function(self) -> Function: ...
    def get_function_expression(self) -> FunctionExpression: ...

class EffectConditionalForall:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_effect(self) -> Effect: ...

class EffectConditionalWhen:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_condition(self) -> Condition: ...
    def get_effect(self) -> Effect: ...

class Effect:
    def get(self) -> Union[EffectLiteral, EffectAnd, EffectNumeric, EffectConditionalForall, EffectConditionalWhen]: ...

class OptimizationMetric:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_function_expression(self) -> FunctionExpression: ...
    def get_optimization_metric(self) -> OptimizationMetricEnum: ...

class PDDLFactories:
    pass

class Action:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_arity(self) -> int: ...
    def get_condition(self) -> Condition: ...
    def get_effect(self) -> Effect: ...
    def get_name(self) -> str: ...
    def get_parameters(self) -> List[Parameter]: ...

class Domain:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_types(self) -> List[Type]: ...
    def get_constants(self) -> List[Object]: ...
    def get_predicates(self) -> List[Predicate]: ...
    def get_fluent_predicates(self) -> List[Predicate]: ...
    def get_static_predicates(self) -> List[Predicate]: ...
    def get_functions(self) -> List[Function]: ...
    def get_actions(self) -> List[Action]: ...

class Problem:
    def __str__(self) -> str: ...
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_domain(self) -> Domain: ...
    def get_requirements(self) -> Requirements: ...
    def get_objects(self) -> List[Object]: ...
    def get_initial_literals(self) -> List[GroundLiteral]: ...
    def get_numeric_fluents(self) -> List[NumericFluent]: ...
    def get_optimization_metric(self) -> OptimizationMetric: ...
    def get_goal_condition(self) -> Condition: ...

class PDDLParser:
    def __init__(self, domain_filepath: str, problem_filepath: str) -> None: ...
    def get_domain(self) -> Domain: ...
    def get_problem(self) -> Problem: ...
    def get_factories(self) -> PDDLFactories: ...


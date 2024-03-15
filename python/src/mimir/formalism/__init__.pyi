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

class Variable:
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...

class Type:
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_bases(self) -> List["Type"]: ...

class Object:
    def get_identifier(self) -> int: ...
    def get_name(self) -> str: ...
    def get_bases(self) -> List[Type]: ...

class Parameter:
    def get_identifier(self) -> int: ...
    def get_variable(self) -> Variable: ...
    def get_bases(self) -> List[Type]: ...

class Predicate:
    def get_name(self) -> str: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_identifier(self) -> int: ...

class TermObject:
    def get_identifier(self) -> int: ...
    def get_object(self) -> Object: ...

class TermVariable:
    def get_identifier(self) -> int: ...
    def get_variable(self) -> Variable: ...

class Term:
    def get(self) -> Union[TermObject, TermVariable]: ...

class Requirements:
    def get_identifier(self) -> int: ...
    def get_requirements(self) -> MutableSet[RequirementEnum]: ...

class Atom:
    def get_identifier(self) -> int: ...
    def get_predicate(self) -> Predicate: ...
    def get_terms(self) -> List[Term]: ...

class Literal:
    def get_identifier(self) -> int: ...
    def get_atom(self) -> Atom: ...
    def is_negated(self) -> bool: ...

class GroundAtom:
    def get_identifier(self) -> int: ...
    def get_predicate(self) -> Predicate: ...
    def get_terms(self) -> List[Term]: ...

class GroundLiteral:
    def get_identifier(self) -> int: ...
    def get_atom(self) -> Atom: ...
    def is_negated(self) -> bool: ...

class FunctionExpressionNumber:
    def get_identifier(self) -> int: ...

class FunctionExpressionBinaryOperator:
    def get_identifier(self) -> int: ...

class FunctionExpressionMultiOperator:
    def get_identifier(self) -> int: ...

class FunctionExpressionMinus:
    def get_identifier(self) -> int: ...

class FunctionExpressionFunction:
    def get_identifier(self) -> int: ...

class FunctionExpression:
    def get(self) -> Union[FunctionExpressionNumber, FunctionExpressionBinaryOperator, FunctionExpressionMultiOperator, FunctionExpressionMinus, FunctionExpressionFunction]: ...

class FunctionSkeleton:
    def get_identifier(self) -> int: ...

class Function:
    def get_identifier(self) -> int: ...
    def get_function_skeleton(self) -> FunctionSkeleton: ...
    def get_terms(self) -> List[Term]: ...

class NumericFluent:
    def get_identifier(self) -> int: ...
    def get_assign_operator(self) -> AssignOperatorEnum: ...
    def get_function(self) -> Function: ...
    def get_function_expression(self) -> FunctionExpression: ...

class ConditionLiteral:
    def get_identifier(self) -> int: ...
    def get_literal(self) -> Literal: ...

class ConditionAnd:
    def get_identifier(self) -> int: ...
    def get_conditions(self) -> List[Condition]: ...

class ConditionOr:
    def get_identifier(self) -> int: ...
    def get_conditions(self) -> List[Condition]: ...

class ConditionNot:
    def get_identifier(self) -> int: ...
    def get_condition(self) -> Condition: ...

class ConditionImply:
    def get_identifier(self) -> int: ...
    def get_condition_left(self) -> Condition: ...
    def get_condition_right(self) -> Condition: ...

class ConditionExists:
    def get_identifier(self) -> int: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_condition(self) -> Condition: ...

class ConditionForall:
    def get_identifier(self) -> int: ...
    def get_parameters(self) -> List[Parameter]: ...
    def get_condition(self) -> Condition: ...

class Condition:
    def get(self) -> Union[ConditionLiteral, ConditionAnd, ConditionOr, ConditionNot, ConditionImply, ConditionExists, ConditionForall]: ...

class EffectLiteral:
    def get_identifier(self) -> int: ...

class EffectAndImpl:
    def get_identifier(self) -> int: ...

class EffectNumericImpl:
    def get_identifier(self) -> int: ...

class EffectConditionalForallImpl:
    def get_identifier(self) -> int: ...

class EffectConditionalWhenImpl:
    def get_identifier(self) -> int: ...

class Effect:
    pass

class OptimizationMetric:
    def get_identifier(self) -> int: ...

class PDDLFactories:
    pass

class Action:
    def get_identifier(self) -> int: ...

class Domain:
    def get_identifier(self) -> int: ...

class Problem:
    def get_identifier(self) -> int: ...

class PDDLParser:
    def __init__(self, domain_filepath: str, problem_filepath: str) -> None: ...
    def get_domain(self) -> Domain: ...
    def get_problem(self) -> Problem: ...
    def get_factories(self) -> PDDLFactories: ...


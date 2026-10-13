# ValtCommand — Runtime Tracker

## Global Syntax

```text
[<type>] line:<line> -> <Message>
```

## Types of Events

- `INFO` — General information about the interpreter.
- `Analysis` — Semantic analysis and validation events.
- `Runtime` — Runtime execution events.
- `WARNING` — Non-fatal issues that may affect execution.
- `ERROR` — Errors that prevent an operation from completing successfully.

---

# Events (Semantic Analysis)

## BinaryOps Node

- `Analysis` — Analyzing binary operation with N operand(s).
- `Analysis` — Checking operand type for binary operator.
- `Analysis` — Operand type is valid for binary operation.
- `Analysis` — Binary operation analysis failed while checking an operand.
- `Analysis` — Binary operation rejected an operand with an unsupported type.
- `Analysis` — Binary operation analysis completed successfully.

## CompOps Node

- `Analysis` — Analyzing comparison operation.
- `Analysis` — Analyzing the left operand.
- `Analysis` — Left operand analysis failed.
- `Analysis` — Analyzing the right operand.
- `Analysis` — Right operand analysis failed.
- `Analysis` — Checking comparison compatibility between operand types.
- `Analysis` — Comparison rejected due to incompatible operand types.
- `Analysis` — Ordered comparison requires numeric operands.
- `Analysis` — Comparison operation passed semantic analysis.

## LogicOps Node

- `Analysis` — Analyzing logical operation with N operand(s).
- `Analysis` — Checking logical operand type; expected `bool`.
- `Analysis` — Logical operand analysis failed.
- `Analysis` — Logical operation rejected an operand with a non-boolean type.
- `Analysis` — Logical operation passed semantic analysis.

## Variable Nodes

### SetVariableNode

- `Analysis` — Analyzing variable declaration.
- `Analysis` — Checking the declared variable type.
- `Analysis` — Analyzing the variable's initial value.
- `Analysis` — Variable declaration analysis failed.
- `Analysis` — Variable declaration passed semantic analysis.

### ResetVariableNode

- `Analysis` — Analyzing variable assignment.
- `Analysis` — Checking whether the target variable exists.
- `Analysis` — Checking assignment type compatibility.
- `Analysis` — Variable assignment analysis failed.
- `Analysis` — Variable assignment passed semantic analysis.

### GetVariableNode

- `Analysis` — Analyzing variable access.
- `Analysis` — Resolving the requested variable.
- `Analysis` — Variable access analysis failed.
- `Analysis` — Variable access passed semantic analysis.

## User Function Nodes

### UserFunctionNode

- `Analysis` — Analyzing user-defined function.
- `Analysis` — Validating function parameters.
- `Analysis` — Analyzing the function body.
- `Analysis` — Function body analysis failed.
- `Analysis` — Validating the function return type.
- `Analysis` — Function analysis completed successfully.

### UserFunctionMethodNode

- `Analysis` — Analyzing a function method or body operation.
- `Analysis` — Validating the method's execution context.
- `Analysis` — Function method analysis failed.
- `Analysis` — Function method analysis completed successfully.

### UserFunctionReturnNode

- `Analysis` — Analyzing return statement.
- `Analysis` — Analyzing the return expression.
- `Analysis` — Checking return type compatibility.
- `Analysis` — Return expression analysis failed.
- `Analysis` — Return type does not match the function's declared return type.
- `Analysis` — Return statement passed semantic analysis.

### UserProxyFunctionNode

- `Analysis` — Analyzing user-function proxy.
- `Analysis` — Resolving the associated user-defined function.
- `Analysis` — User-function proxy analysis failed.
- `Analysis` — User-function proxy analysis completed successfully.

## Loop Nodes

### WhileLoopNode

- `Analysis` — Analyzing while loop.
- `Analysis` — Analyzing the loop condition.
- `Analysis` — Checking that the loop condition is boolean.
- `Analysis` — Loop condition analysis failed.
- `Analysis` — Loop body analysis failed.
- `Analysis` — While loop passed semantic analysis.

### ForLoopNode

- `Analysis` — Analyzing for loop.
- `Analysis` — Analyzing loop initialization.
- `Analysis` — Analyzing loop condition.
- `Analysis` — Analyzing loop iteration expression.
- `Analysis` — Checking that the loop condition is boolean.
- `Analysis` — For-loop component analysis failed.
- `Analysis` — For loop passed semantic analysis.

### BreakLoopNode

- `Analysis` — Analyzing break statement.
- `Analysis` — Checking whether break is used inside a loop.
- `Analysis` — Break statement is outside a valid loop context.
- `Analysis` — Break statement passed semantic analysis.

### ContinueLoopNode

- `Analysis` — Analyzing continue statement.
- `Analysis` — Checking whether continue is used inside a loop.
- `Analysis` — Continue statement is outside a valid loop context.
- `Analysis` — Continue statement passed semantic analysis.

---

# Events (Runtime Execution)

## BinaryOps Node

- `Runtime` — Executing binary operation.
- `Runtime` — Evaluating operand for an operator.
- `Runtime` — Binary operation failed while evaluating an operand.
- `Runtime` — Division by zero detected.
- `Runtime` — Applied an operator.
- `Runtime` — Binary operation completed successfully.

## CompOps Node

- `Runtime` — Executing comparison operation.
- `Runtime` — Comparison failed while evaluating the left operand.
- `Runtime` — Comparison failed while evaluating the right operand.
- `Runtime` — Cannot execute an ordered comparison with non-numeric operands.
- `Runtime` — Comparison evaluated successfully.

## LogicOps Node

- `Runtime` — Executing logical operation.
- `Runtime` — Logical operation failed while evaluating the first operand.
- `Runtime` — Evaluating a logical operator (`&&` or `||`).
- `Runtime` — Logical operation failed while evaluating an operand.
- `Runtime` — Logical operation received a non-boolean operand.
- `Runtime` — Logical operation completed successfully.

## Variable Nodes

### SetVariableNode

- `Runtime` — Executing variable declaration.
- `Runtime` — Evaluating the variable's initial value.
- `Runtime` — Failed to evaluate the variable's initial value.
- `Runtime` — Variable declared successfully.

### ResetVariableNode

- `Runtime` — Executing variable assignment.
- `Runtime` — Evaluating the new variable value.
- `Runtime` — Failed to evaluate the assigned value.
- `Runtime` — Variable assignment completed successfully.

### GetVariableNode

- `Runtime` — Reading variable value.
- `Runtime` — Variable lookup failed.
- `Runtime` — Variable value retrieved successfully.

## User Function Nodes

### UserFunctionNode

- `Runtime` — Executing user-defined function.
- `Runtime` — Creating the function execution scope.
- `Runtime` — Executing the function body.
- `Runtime` — Function execution failed.
- `Runtime` — Function execution completed successfully.

### UserFunctionMethodNode

- `Runtime` — Executing function method.
- `Runtime` — Function method execution failed.
- `Runtime` — Function method executed successfully.

### UserFunctionReturnNode

- `Runtime` — Executing return statement.
- `Runtime` — Evaluating return expression.
- `Runtime` — Return expression evaluation failed.
- `Runtime` — Returning a value from the function.
- `Runtime` — Return statement completed successfully.

### UserProxyFunctionNode

- `Runtime` — Executing user-function proxy.
- `Runtime` — Forwarding execution to the associated function.
- `Runtime` — User-function proxy execution failed.
- `Runtime` — User-function proxy execution completed successfully.

## Loop Nodes

### WhileLoopNode

- `Runtime` — Entering while loop.
- `Runtime` — Evaluating while-loop condition.
- `Runtime` — While-loop condition evaluation failed.
- `Runtime` — While-loop condition evaluated to false; exiting loop.
- `Runtime` — Executing while-loop body.
- `Runtime` — While-loop body execution failed.
- `Runtime` — While loop completed.

### ForLoopNode

- `Runtime` — Entering for loop.
- `Runtime` — Executing loop initialization.
- `Runtime` — Evaluating for-loop condition.
- `Runtime` — For-loop condition evaluation failed.
- `Runtime` — For-loop condition evaluated to false; exiting loop.
- `Runtime` — Executing for-loop body.
- `Runtime` — Executing for-loop iteration expression.
- `Runtime` — For-loop execution failed.
- `Runtime` — For loop completed.

### BreakLoopNode

- `Runtime` — Executing break statement.
- `Runtime` — Exiting the current loop using break.

### ContinueLoopNode

- `Runtime` — Executing continue statement.
- `Runtime` — Skipping the remaining loop body using continue.
- `Runtime` — Continuing with the next loop iteration.

---

# Recommended Additional Events

These events should only be documented as implemented after confirming that the corresponding code emits them.

## Function Calls

- `Analysis` — Analyzing function call.
- `Analysis` — Resolving function name.
- `Analysis` — Validating argument count.
- `Analysis` — Validating argument types.
- `Analysis` — Function call analysis failed.
- `Analysis` — Function call passed semantic analysis.
- `Runtime` — Executing function call.
- `Runtime` — Evaluating function arguments.
- `Runtime` — Function argument evaluation failed.
- `Runtime` — Function call completed successfully.

## Errors and Runtime State

- `ERROR` — Name resolution failed.
- `ERROR` — Type validation failed.
- `ERROR` — Invalid function arguments.
- `ERROR` — Runtime operation failed.
- `WARNING` — Runtime operation produced a potentially unexpected result.

## Debugger Lifecycle

- `INFO` — Semantic analysis started.
- `INFO` — Runtime execution started.
- `INFO` — Semantic analysis completed.
- `INFO` — Runtime execution completed.
- `INFO` — Runtime execution terminated with an error.


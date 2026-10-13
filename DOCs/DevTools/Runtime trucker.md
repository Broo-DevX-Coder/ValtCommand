# The runtime trucker

## Gloal syntax

```
[<type>] line:<line> -> <Message> 
```

## Types of events
- INFO: General info
- 

## Events (Semantic Analysis)

### BinaryOps Node

- `Analysis` — Analyzing binary operation with N operand(s).
- `Analysis` — Checking operand type for binary operator.
- `Analysis` — Operand type is valid for binary operation.
- `Analysis` — Binary operation analysis failed while checking an operand.
- `Analysis` — Binary operation rejected an operand with an unsupported type.
- `Analysis` — Binary operation analysis completed successfully.

### CompOps Node

- `Analysis` — Analyzing comparison operation.
- `Analysis` — Analyzing the left operand.
- `Analysis` — Left operand analysis failed.
- `Analysis` — Analyzing the right operand.
- `Analysis` — Right operand analysis failed.
- `Analysis` — Checking comparison compatibility between operand types.
- `Analysis` — Comparison rejected due to incompatible operand types.
- `Analysis` — Ordered comparison requires numeric operands.
- `Analysis` — Comparison operation passed semantic analysis.

### LogicOps Node

- `Analysis` — Analyzing logical operation with N operand(s).
- `Analysis` — Checking logical operand type; expected `bool`.
- `Analysis` — Logical operand analysis failed.
- `Analysis` — Logical operation rejected an operand with a non-boolean type.
- `Analysis` — Logical operation passed semantic analysis.

## Events (Runtime Execution)

### BinaryOps Node

- `Runtime` — Executing binary operation.
- `Runtime` — Evaluating operand for an operator.
- `Runtime` — Binary operation failed while evaluating an operand.
- `Runtime` — Division by zero detected.
- `Runtime` — Applied an operator.
- `Runtime` — Binary operation completed successfully.

### CompOps Node

- `Runtime` — Executing comparison operation.
- `Runtime` — Comparison failed while evaluating the left operand.
- `Runtime` — Comparison failed while evaluating the right operand.
- `Runtime` — Cannot execute an ordered comparison with non-numeric operands.
- `Runtime` — Comparison evaluated successfully.

### LogicOps Node

- `Runtime` — Executing logical operation.
- `Runtime` — Logical operation failed while evaluating the first operand.
- `Runtime` — Evaluating a logical operator (`&&` or `||`).
- `Runtime` — Logical operation failed while evaluating an operand.
- `Runtime` — Logical operation received a non-boolean operand.
- `Runtime` — Logical operation completed successfully.

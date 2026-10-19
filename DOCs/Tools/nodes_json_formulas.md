# ValtCommand AST JSON Formats

Reference guide for the JSON structures returned by each AST node's `nlohmann::json get_json()` method. The C++ formulas below include the requested comment above each function.

## Conventions

- Every node is represented by a JSON object with a `type` field.
- Child AST nodes are embedded as JSON objects by calling their `get_json()` method.
- Ordered collections use JSON arrays, preserving source/AST order.
- Example child nodes and values are illustrative; their exact fields depend on the corresponding node implementation.
- These implementations assume child pointers are valid except where the original `get_str()` explicitly checked for null.

---

## 1. `BinOpsNode`

Represents a sequence of binary-operation parts. The first part has the role `First`; subsequent parts include their operator.

```cpp
// Get the json file of ASTNode
nlohmann::json
BinOpsNode::get_json() {
    nlohmann::json result;

    result["type"] = "BinaryOperation";
    result["parts"] = nlohmann::json::array();

    size_t part_i = 0;

    for (const auto& part : Parts) {
        nlohmann::json part_json;

        if (part_i == 0)
            part_json["role"] = "First";
        else
            part_json["operator"] = binary_to_string(part.op);

        part_json["node"] = part.node->get_json();

        result["parts"].push_back(part_json);
        ++part_i;
    }

    return result;
}
```

JSON shape:

```json
{
  "type": "BinaryOperation",
  "parts": [
    { "role": "First", "node": { "type": "Number", "value": 5 } },
    { "operator": "+", "node": { "type": "Number", "value": 3 } }
  ]
}
```

Fields: `type` identifies the node; `parts` is the ordered list; each part contains a child `node` and either `role` or `operator`.

## 2. `CompOpsNode`

Represents a comparison between two expressions.

```cpp
// Get the json file of ASTNode
nlohmann::json
CompOpsNode::get_json() {
    nlohmann::json result;

    result["type"] = "ComparisonOperation";
    result["operator"] = comparison_to_string(type);

    result["first_part"] = first_part.node->get_json();
    result["second_part"] = second_part.node->get_json();

    return result;
}
```

JSON shape:

```json
{
  "type": "ComparisonOperation",
  "operator": ">",
  "first_part": { "type": "Number", "value": 10 },
  "second_part": { "type": "Number", "value": 5 }
}
```

Fields: `operator` is returned by `comparison_to_string(type)`; `first_part` and `second_part` are the left and right expression nodes.

## 3. `LogicOpsNode`

Represents a sequence of logical operands and operators.

```cpp
// Get the json file of ASTNode
nlohmann::json
LogicOpsNode::get_json() {
    nlohmann::json result;

    result["type"] = "LogicalOperation";
    result["parts"] = nlohmann::json::array();

    for (const auto& part : Parts) {
        nlohmann::json part_json;

        if (part.op == TokenType::LOGICAL_AND)
            part_json["operator"] = "AND &&";
        else if (part.op == TokenType::LOGICAL_OR)
            part_json["operator"] = "OR ||";
        else
            part_json["role"] = "First";

        part_json["node"] = part.node->get_json();
        result["parts"].push_back(part_json);
    }

    return result;
}
```

JSON shape:

```json
{
  "type": "LogicalOperation",
  "parts": [
    { "role": "First", "node": { "type": "Boolean", "value": true } },
    { "operator": "AND &&", "node": { "type": "Boolean", "value": false } }
  ]
}
```

Fields: `parts` preserves operand order. The first part has `role`; later parts use `operator` (`AND &&` or `OR ||`).

## 4. `PowerNode`

Represents exponentiation.

```cpp
// Get the json file of ASTNode
nlohmann::json
PowerNode::get_json() {
    nlohmann::json result;

    result["type"] = "PowerOperation";
    result["operator"] = "**";
    result["base"] = left.node->get_json();
    result["exponent"] = right.node->get_json();

    return result;
}
```

JSON shape:

```json
{
  "type": "PowerOperation",
  "operator": "**",
  "base": { "type": "Number", "value": 2 },
  "exponent": { "type": "Number", "value": 3 }
}
```

Fields: `base` is the left operand; `exponent` is the right operand.

## 5. `FunctionCallArgumentNode`

Represents one named argument supplied to a function call.

```cpp
// Get the json file of ASTNode
nlohmann::json
FunctionCallArgumentNode::get_json() {
    nlohmann::json result;

    result["type"] = "FunctionCallArgument";
    result["name"] = name;
    result["value"] = VNode->get_json();

    return result;
}
```

JSON shape:

```json
{
  "type": "FunctionCallArgument",
  "name": "exponent",
  "value": { "type": "Number", "value": 3 }
}
```

Fields: `name` is the argument name; `value` is the JSON representation of its expression.

## 6. `FunctionCallNode`

Represents a function call and its arguments.

```cpp
// Get the json file of ASTNode
nlohmann::json
FunctionCallNode::get_json() {
    nlohmann::json result;

    result["type"] = "FunctionCall";
    result["name"] = name;
    result["arguments"] = nlohmann::json::array();

    for (const auto& arg : arguments)
        result["arguments"].push_back(arg->get_json());

    return result;
}
```

JSON shape:

```json
{
  "type": "FunctionCall",
  "name": "power",
  "arguments": [
    { "type": "FunctionCallArgument", "name": "base", "value": { "type": "Number", "value": 2 } },
    { "type": "FunctionCallArgument", "name": "exponent", "value": { "type": "Number", "value": 3 } }
  ]
}
```

Fields: `name` is the called function; `arguments` is an ordered array and may be empty.

## 7. `IfStatmentsNode`

Represents an if statement with a condition, the main body, and an else body.

```cpp
// Get the json file of ASTNode
nlohmann::json
IfStatmentsNode::get_json() {
    nlohmann::json result;

    result["type"] = "IfStatement";
    result["condition"] = condition_node->get_json();
    result["statements"] = nlohmann::json::array();

    for (const auto& stm : Statments)
        result["statements"].push_back(stm->get_json());

    result["else_statements"] = nlohmann::json::array();

    for (const auto& stm : Else_statments)
        result["else_statements"].push_back(stm->get_json());

    return result;
}
```

JSON shape:

```json
{
  "type": "IfStatement",
  "condition": { "type": "Boolean", "value": true },
  "statements": [],
  "else_statements": []
}
```

Fields: `condition` is the condition expression; `statements` contains the if body; `else_statements` contains the else body and is present even when empty.

## 8. `BreakLoopNode`

Represents a `break` statement.

```cpp
// Get the json file of ASTNode
nlohmann::json
BreakLoopNode::get_json() {
    nlohmann::json result;
    result["type"] = "Break";
    return result;
}
```

JSON shape: `{ "type": "Break" }`.

## 9. `ContinueLoopNode`

Represents a `continue` statement.

```cpp
// Get the json file of ASTNode
nlohmann::json
ContinueLoopNode::get_json() {
    nlohmann::json result;
    result["type"] = "Continue";
    return result;
}
```

JSON shape: `{ "type": "Continue" }`.

## 10. `WhileLoopsNode`

Represents a while loop.

```cpp
// Get the json file of ASTNode
nlohmann::json
WhileLoopsNode::get_json() {
    nlohmann::json result;

    result["type"] = "WhileLoop";
    result["condition"] = condition_node->get_json();
    result["statements"] = nlohmann::json::array();

    for (const auto& stm : Statments)
        result["statements"].push_back(stm->get_json());

    return result;
}
```

JSON shape:

```json
{
  "type": "WhileLoop",
  "condition": { "type": "Boolean", "value": true },
  "statements": []
}
```

Fields: `condition` is the loop condition; `statements` is the ordered loop body.

## 11. `ForLoopsNode`

Represents a for loop with initialization, condition, update, and body.

```cpp
// Get the json file of ASTNode
nlohmann::json
ForLoopsNode::get_json() {
    nlohmann::json result;

    result["type"] = "ForLoop";
    result["initialization"] = Initialization.node->get_json();
    result["condition"] = Condition.node->get_json();
    result["update"] = Update.node->get_json();
    result["statements"] = nlohmann::json::array();

    for (const auto& stm : Statments)
        result["statements"].push_back(stm->get_json());

    return result;
}
```

JSON shape:

```json
{
  "type": "ForLoop",
  "initialization": { "type": "VariableDeclaration" },
  "condition": { "type": "ComparisonOperation" },
  "update": { "type": "VariableAssignment" },
  "statements": []
}
```

Fields: `initialization`, `condition`, and `update` represent the corresponding loop components; `statements` is the body.

## 12. `ModuleNode`

Represents the complete program/module.

```cpp
// Get the json file of ASTNode
nlohmann::json
ModuleNode::get_json() {
    nlohmann::json result;

    result["type"] = "Program";
    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement.node)
            result["statements"].push_back(statement.node->get_json());
    }

    return result;
}
```

JSON shape: `{ "type": "Program", "statements": [] }`.

Fields: `statements` contains non-null top-level nodes in order; entries with a null `statement.node` are skipped.

## 13. `SignOpNode`

Represents a unary sign, such as `+x` or `-x`.

```cpp
// Get the json file of ASTNode
nlohmann::json
SignOpNode::get_json() {
    nlohmann::json result;

    result["type"] = "SignOperation";
    result["operator"] = sign_to_string(type);

    if (node)
        result["operand"] = node->get_json();
    else
        result["operand"] = nullptr;

    return result;
}
```

JSON shape:

```json
{
  "type": "SignOperation",
  "operator": "-",
  "operand": { "type": "Number", "value": 5 }
}
```

Fields: `operator` comes from `sign_to_string(type)`; `operand` is the child node or `null`.

## 14. `LogicalNotNode`

Represents unary logical NOT.

```cpp
// Get the json file of ASTNode
nlohmann::json
LogicalNotNode::get_json() {
    nlohmann::json result;

    result["type"] = "LogicalNot";
    result["operator"] = "!";
    result["operand"] = node->get_json();

    return result;
}
```

JSON shape:

```json
{
  "type": "LogicalNot",
  "operator": "!",
  "operand": { "type": "Boolean", "value": true }
}
```

Note: this formula assumes `node` is non-null, matching the original `get_str()` implementation.

## 15. `UserFunctionMethodNode`

Represents a parameter/method entry in a user-defined function signature.

```cpp
// Get the json file of ASTNode
nlohmann::json
UserFunctionMethodNode::get_json() {
    nlohmann::json result;

    result["type"] = "UserFunctionMethod";
    result["name"] = name_token.value;
    result["return_type"] = type_token.value;

    if (default_value_node != nullptr)
        result["default_value"] = default_value_node->get_json();
    else
        result["default_value"] = nullptr;

    return result;
}
```

JSON shape:

```json
{
  "type": "UserFunctionMethod",
  "name": "exponent",
  "return_type": "int",
  "default_value": null
}
```

**Naming note:** `type_token.value` appears to represent the parameter's type. If so, `"type"` would be a more accurate field name than `"return_type"`. The formula above preserves the earlier agreed version.

## 16. `UserFunctionReturnNode`

Represents a return statement.

```cpp
// Get the json file of ASTNode
nlohmann::json
UserFunctionReturnNode::get_json() {
    nlohmann::json result;

    result["type"] = "Return";

    if (VNode != nullptr)
        result["value"] = VNode->get_json();
    else
        result["value"] = nullptr;

    return result;
}
```

JSON shape: `{ "type": "Return", "value": null }`.

Fields: `value` is the returned expression or `null` for a return without a value.

## 17. `UserProxyFunctionNode`

Represents a proxy/wrapper node containing function statements.

```cpp
// Get the json file of ASTNode
nlohmann::json
UserProxyFunctionNode::get_json() {
    nlohmann::json result;

    result["type"] = "UserFunctionProxy";
    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement != nullptr)
            result["statements"].push_back(statement->get_json());
    }

    return result;
}
```

JSON shape: `{ "type": "UserFunctionProxy", "statements": [] }`.

Fields: `statements` contains non-null child nodes in order.

## 18. `UserFunctionNode`

Represents a user-defined function declaration, its signature, and its body.

```cpp
// Get the json file of ASTNode
nlohmann::json
UserFunctionNode::get_json() {
    nlohmann::json result;

    result["type"] = "UserFunction";
    result["name"] = name_token.value;
    result["return_type"] = return_type_token.value;
    result["methods"] = nlohmann::json::array();

    for (const auto& method : methods_list) {
        if (method != nullptr)
            result["methods"].push_back(method->get_json());
    }

    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement != nullptr)
            result["statements"].push_back(statement->get_json());
    }

    return result;
}
```

JSON shape:

```json
{
  "type": "UserFunction",
  "name": "power",
  "return_type": "float",
  "methods": [],
  "statements": []
}
```

Fields: `name` is the function name; `return_type` is the function return type; `methods` lists parameters/method entries; `statements` lists the body nodes.

## 19. `ValueNode`

Represents a literal/token value.

```cpp
// Get the json file of ASTNode
nlohmann::json
ValueNode::get_json() {
    nlohmann::json result;

    result["type"] = "Value";
    result["value_type"] = type;
    result["value"] = vToken.value;

    return result;
}
```

JSON shape if `vToken.value` is a string:

```json
{
  "type": "Value",
  "value_type": "int",
  "value": "42"
}
```

Fields: `value_type` stores the node's `type`; `value` stores `vToken.value`. Its JSON type depends on the C++ type of `vToken.value`. If that member is a `std::string`, numeric-looking values serialize as JSON strings, not numbers.

## 20. `SetVariableNode`

Represents a variable or constant declaration.

```cpp
// Get the json file of ASTNode
nlohmann::json
SetVariableNode::get_json() {
    nlohmann::json result;

    result["type"] = is_const ? "ConstantDeclaration"
                              : "VariableDeclaration";
    result["name"] = name;
    result["value_type"] = type;
    result["value"] = VNode->get_json();

    return result;
}
```

JSON shape:

```json
{
  "type": "VariableDeclaration",
  "name": "x",
  "value_type": "int",
  "value": { "type": "Value", "value_type": "int", "value": "10" }
}
```

Fields: `type` is `VariableDeclaration` or `ConstantDeclaration` according to `is_const`; `name` is the declared name; `value_type` is the declared type; `value` is the initializer AST.

## 21. `ResetVariableNode`

Represents assignment to an existing variable.

```cpp
// Get the json file of ASTNode
nlohmann::json
ResetVariableNode::get_json() {
    nlohmann::json result;

    result["type"] = "VariableAssignment";
    result["name"] = name;
    result["value"] = VNode->get_json();

    return result;
}
```

JSON shape:

```json
{
  "type": "VariableAssignment",
  "name": "x",
  "value": { "type": "Value", "value_type": "int", "value": "20" }
}
```

Fields: `name` is the target variable; `value` is the assigned expression AST.

## 22. `GetVariableNode`

Represents reading/accessing a variable by name.

```cpp
// Get the json file of ASTNode
nlohmann::json
GetVariableNode::get_json() {
    nlohmann::json result;

    result["type"] = "VariableAccess";
    result["name"] = name;

    return result;
}
```

JSON shape: `{ "type": "VariableAccess", "name": "x" }`.

Fields: `name` is the variable being accessed.

---

## Node index

| C++ node | JSON `type` | Main fields |
|---|---|---|
| `BinOpsNode` | `BinaryOperation` | `parts` |
| `CompOpsNode` | `ComparisonOperation` | `operator`, `first_part`, `second_part` |
| `LogicOpsNode` | `LogicalOperation` | `parts` |
| `PowerNode` | `PowerOperation` | `operator`, `base`, `exponent` |
| `FunctionCallArgumentNode` | `FunctionCallArgument` | `name`, `value` |
| `FunctionCallNode` | `FunctionCall` | `name`, `arguments` |
| `IfStatmentsNode` | `IfStatement` | `condition`, `statements`, `else_statements` |
| `BreakLoopNode` | `Break` | — |
| `ContinueLoopNode` | `Continue` | — |
| `WhileLoopsNode` | `WhileLoop` | `condition`, `statements` |
| `ForLoopsNode` | `ForLoop` | `initialization`, `condition`, `update`, `statements` |
| `ModuleNode` | `Program` | `statements` |
| `SignOpNode` | `SignOperation` | `operator`, `operand` |
| `LogicalNotNode` | `LogicalNot` | `operator`, `operand` |
| `UserFunctionMethodNode` | `UserFunctionMethod` | `name`, `return_type`, `default_value` |
| `UserFunctionReturnNode` | `Return` | `value` |
| `UserProxyFunctionNode` | `UserFunctionProxy` | `statements` |
| `UserFunctionNode` | `UserFunction` | `name`, `return_type`, `methods`, `statements` |
| `ValueNode` | `Value` | `value_type`, `value` |
| `SetVariableNode` | `VariableDeclaration` / `ConstantDeclaration` | `name`, `value_type`, `value` |
| `ResetVariableNode` | `VariableAssignment` | `name`, `value` |
| `GetVariableNode` | `VariableAccess` | `name` |

## Integration checklist

1. Include `#include <nlohmann/json.hpp>` where needed.
2. Make sure declarations in headers match the `nlohmann::json get_json()` definitions.
3. If `get_json()` is called through a base-class pointer, declare it appropriately as a virtual method in the base class.
4. Validate child pointers before dereferencing them if null pointers are possible. The formulas only add null handling where the original `get_str()` did so.
5. Confirm the actual C++ type of `vToken.value` and decide whether JSON literals should be numbers/booleans rather than strings.
6. Confirm whether `UserFunctionMethodNode` should name its `type_token.value` field `type` rather than `return_type`.
7. Treat field names and `type` labels as an API once AST inspectors or other tools consume this JSON.

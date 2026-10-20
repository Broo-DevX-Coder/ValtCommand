# Roadmap

| Version | Goal |
|----------|----------|
| v0.1 | Core language foundation |
| v0.2 | Semantic analysis and language validation |
| v0.3 | Language features and expressions |
| v0.4 | Developer tools and debugging |
| v1.0 | Stable public release |

---

## v0.1

- ✅ Lexer
- ✅ Parser
- ✅ AST
- ✅ Token Inspector
- ✅ AST Inspector
- ✅ Function Registry
- ✅ Value Nodes
- ✅ Basic Type Validation
- ✅ Basic Error Handling

---

## v0.2

- ✅ Runtime Engine
- ✅ Scope System
- ✅ Block Scopes
- ✅ Recursive Parser
- ✅ Semantic Analysis
- ✅ Function Resolution
- ✅ Argument Validation
- ✅ Required Arguments Validation
- ✅ Argument Type Validation
- ✅ Nested Function Calls
- ✅ Type Compatibility Checking
- ✅ Improved Error Reporting
- ✅ Additional Tokens Support

---

## v0.3

- ✅ Comments Support
- ✅ Variables
- ✅ Constants
- ✅ ///Return Values
- ✅ Binary Operations (`+ - * / ^`)
- ✅ Operator Precedence
- ✅ Parentheses Expressions
- ✅ User Functions (`FUNCTION`)
- ✅ Comparition operations (`==`,`!=`,`>`,`<`,`>=`,`<=`)
- ✅ Create `&&` (AND) and `||` (OR)
- ✅ Recursive Comparition operations `(55 == 5 || ((66 != 2) && 5 == 5) )`
- ✅ IF Statements
- ✅ ELSE Statements
- ✅ WHILE Loops
- ✅ FOR Loops

---

## v0.4

### Debugging

- ✅ Better error handling
- ✅ Runtime Tracer
- ✅ Create a get_json in ASTNodes
- ✅ Scope Inspector
- ✅ Variable Inspector
- ✅ Function Inspector
- ✅ Crete nodes of `INSPECT__SCOPE__` + `INSPECT__VARS__` + `INSPECT__FUNCS__` to use inspectors diractelly in code

### Developer Tools

- Debug Tools
- Performance Metrics
- Better Diagnostics

### Embedded Standard Library

#### Mathematical Constants
- Add mathematical constants (`PI`, `E`, `TAU`, `INF`, `NAN`)
- Add mathematical constants documentation

#### Basic Math Functions
- Add basic math functions (`abs`, `sqrt`, `cbrt`, `pow`, `root`)
- Add rounding functions (`floor`, `ceil`, `round`, `trunc`)
- Add remainder and division functions (`fmod`, `divmod`)
- Add min/max functions (`min`, `max`, `clamp`)
- Add sign function (`sign`)

#### Power and Logarithmic Functions
- Add exponential functions (`exp`, `exp2`)
- Add logarithmic functions (`log`, `log2`, `log10`, `log1p`)

#### Trigonometric Functions
- Add trigonometric functions (`sin`, `cos`, `tan`)
- Add inverse trigonometric functions (`asin`, `acos`, `atan`, `atan2`)
- Add hyperbolic functions (`sinh`, `cosh`, `tanh`)
- Add angle conversion functions (`degrees`, `radians`)

#### Numeric Utilities
- Add floating-point classification functions (`isnan`, `isinf`, `isfinite`)
- Add numeric limits and type information
- Define consistent behavior for domain errors, overflow, and invalid arguments

#### Random Number Functions
- Add random number functions (`random`, `randint`)
- Add random seed configuration (`seed_random`)

---
#### __Note__: Embedded Function Infrastructure
- Define consistent argument and return types for built-in functions
- Validate argument counts and types
- Integrate built-in functions with the Function Registry
- Add unit tests for built-in functions
- Document all built-in functions and mathematical constants
---

### Documentation

- Language Reference
- Standard Library Documentation
- Embedding Guide
- Examples & Tutorials

---

## v1.0

- Stable API
- Complete Documentation
- Performance Optimizations
- Production-Ready Release
- Long-Term Compatibility

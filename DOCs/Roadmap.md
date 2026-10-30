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
- ✅ Create `get_json` in AST nodes
- ✅ Scope Inspector
- ✅ Variable Inspector
- ✅ Function Inspector
- ✅ Create `INSPECT__SCOPE__`, `INSPECT__VARS__` and `INSPECT__FUNCS__` nodes to use inspectors directly in code

---

### Runtime / Embedding API

- ✅ Add `runtime.run(code)` to execute code from a string
- ✅ Add `runtime.run_file(path)` to execute a script file
- ✅ Return a result object (value, error, error line/column) instead of crashing the host
- ✅ Keep state between calls (variables and functions persist), with an option for a fresh scope
- ✅ Add `runtime.reset()` to clear the state
- ✅ Allow the host (C++) to register native functions into the Function Registry
- ✅ Allow the host to read and set variables in the global scop

---

### Embedded Standard Library

#### Function Infrastructure
- ✅ Define consistent argument and return types for built-in functions
- ✅ Validate argument counts and types
- ✅ Integrate built-in functions with the Function Registry
- ✅ Define consistent behavior for domain errors, overflow, and invalid arguments
- ✅ Add unit tests for built-in functions

#### Input / Output Functions
- ✅ Add output functions (`print`, `println`)
- ✅ Add input function (`input`)
- ✅ Define behavior for multiple arguments, separators, and line endings

#### Type Conversion Functions
- ✅ Add conversion functions (`int`, `float`, `str`, `bool`)
- ✅ Add type inspection function (`type`)
- ✅ Define behavior for invalid conversions (e.g. `int("abc")`)

#### Mathematical Constants
- ✅ Add mathematical constants (`PI`, `E`, `TAU`, `INF`, `NAN`)

#### Basic Math Functions
- ✅ Add basic math functions (`abs`, `sqrt`, `cbrt`, `pow`, `root`)
- ✅ Add rounding functions (`floor`, `ceil`, `round`, `trunc`)
- ✅ Add remainder and division functions (`fmod`, `divmod`)
- ✅ Add min/max functions (`min`, `max`, `clamp`)
- ✅ Add sign function (`sign`)

#### Power and Logarithmic Functions
- ✅ Add exponential functions (`exp`, `exp2`)
- ✅ Add logarithmic functions (`log`, `log2`, `log10`, `log1p`)

#### Trigonometric Functions
- ✅ Add trigonometric functions (`sin`, `cos`, `tan`)
- ✅ Add inverse trigonometric functions (`asin`, `acos`, `atan`, `atan2`)
- ✅ Add hyperbolic functions (`sinh`, `cosh`, `tanh`)
- ✅ Add angle conversion functions (`degrees`, `radians`)

#### Numeric Utilities
- ✅ Add floating-point classification functions (`isnan`, `isinf`, `isfinite`)
- ✅ Add numeric limits and type information

#### Random Number Functions
- ✅ Add random number functions (`random`, `randint`)
- ✅ Add random seed configuration (`seed_random`)

#### Format Function
- ✅ Add `format` function (e.g. `format("Hello {}", name)`)
- ✅ Support positional placeholders and number formatting (precision, padding)
- ✅ Define behavior for missing or extra arguments

---

### Developer Tools

- ✅ Debugger with breakpoints
- ✅ Step-by-step execution
- ✅ Inspect the active scope and display its variables and functions at a breakpoint
- ✅ Expose tracker callbacks, event retrieval, breakpoint management, stop callbacks, output streams, and stepping controls through the Runtime API
- ✅ Include source file, line, and active scope in AST runtime events
- ✅ Support file-specific breakpoints and temporary next-line breakpoints
- Performance Metrics
- Better Diagnostics

### Final fixes

---

### Documentation

- ✅ Runtime debugger, event tracker, breakpoint, and inspector guide (`DOCs/DevTools/Runtime_Debugger_and_Inspectors.md`)
- Language Reference
- Standard Library Documentation (all built-in functions and mathematical constants)
- Embedding Guide
- Examples & Tutorials

---

## v1.0

- Stable API
- Complete Documentation
- Performance Optimizations
- Production-Ready Release
- Long-Term Compatibility
# ValtCommand 1.0.0 documentation

**Version:** 1.0.0

These pages describe the ValtCommand language and the C++ embedding API.

## C++ namespace

All declarations from the project headers are rooted in `vltc`. Existing API groupings remain nested under that root: for example, create a runtime with `vltc::Runtime::RunTime`, inspect results with `vltc::Runtime::RunResult`, and use scope types from `vltc::Scopes`.

Include the public runtime header with `#include "valtcommand/runtime.hpp"`. The convenience header `#include "valtcommand/valtcommand.hpp"` includes the runtime, scope, and debugger interfaces.

## Language reference

- [Lexical structure](lexical-structure.md)
- [Grammar](grammar.md)
- [Types and values](types-and-values.md)
- [Variables](variables.md)
- [Statements](statements.md)
- [Expressions and operators](expressions-and-operators.md)
- [Functions](functions.md)
- [Loops](loops.md)
- [Built-in functions](built-in-functions.md)
- [Errors](errors.md)
- [Known limitations](limitations.md)

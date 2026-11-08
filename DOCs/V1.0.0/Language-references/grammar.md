# Grammar

**Version:** 1.0.0

This reference describes the syntax accepted by the current lexer and parser. Each production names the parser method that implements it. Semantic checks are listed separately.

## 1. EBNF notation

- `A ::= B` defines `A`.
- `|` means alternative.
- Adjacent items are concatenated in order.
- `{ X }` means zero or more repetitions of `X`.
- `[ X ]` means zero or one occurrence of `X`.
- Text in double quotes is a literal source token.
- `letter`, `digit`, and `any-character` are lexical descriptions, not ValtCommand tokens.

The productions describe token order. Before tokenization, the lexer discards whitespace and removes `//` line comments and `/* ... */` block comments from the source; the parser receives tokens after that removal. Except for `FOR` header semicolons, statements do not need separators. The parser uses a `do`-style module loop, so it attempts to parse at least one statement; an empty file is not accepted.

## 2. Lexical structure

The lexer implementation is `vltc::Lexar::get_the_next_token` in `src/Lexar.cpp`; token sets and recognized words are defined in `src/Globals.cpp`.

```ebnf
identifier ::= ( letter | "_" ) { letter | digit | "_" }
number ::= digit { digit } [ "." { digit } ]
string ::= string-token
boolean ::= "True" | "False"
type ::= "str" | "int" | "float" | "bool" | "void" | "any"
line-comment ::= "//" { any-character-except-newline }
block-comment ::= "/*" { any-character } "*/"
```

`identifier` uses ASCII letters or `_` first; later characters may also be ASCII decimal digits. `number` begins with a digit, may contain one decimal point, and may have no digits after that point (for example, `12.` is lexed as a float). Exponent notation is not recognized.

`string-token` is delimited by matching single or double quotes and may contain whitespace except a line feed. The lexer maps `\n`, `\t`, `\\`, `\"`, and `\'` to their corresponding characters; unknown escapes preserve the backslash and following character. A line feed or end of input before the closing quote emits an unterminated-string lexical error (`vltc::Lexar::get_the_next_token`).

`//` removes all following characters on the same line through the next newline or end of input. `/* ... */` removes everything through the first closing marker; block comments are not nested. An unterminated block comment emits an `UNTERMINATED_COMMENT` token and causes a parser `SyntaxError` with a dedicated message (`vltc::Lexar::escape_comments`, `vltc::Lexar::get_the_next_token`).

The lexer emits two-character tokens for `==`, `!=`, `<=`, `>=`, `&&`, `||`, `%%`, and `**`. The sequence `->` is not a token: it is lexed as `-` followed by `>`. Single-character tokens include `+ - * / % = ! < > : ; ( ) [ ] { } , \ ^`. The parser uses `END` as a block terminator. Recognized keywords are `CALL`, `SET`, `SET_CONST`, `GET`, `FUNCTION`, `RETURN`, `IF`, `ELSE`, `THEN`, `WHILE`, `FOR`, `BREAK`, and `CONTINUE`. The recognized type names are listed in `type` above.

## 3. Programs and statements

`vltc::Parser::get_module_node` repeatedly parses expressions until end of input. `vltc::Parser::get_expretion` is the expression entry point; `vltc::Parser::get_primary` dispatches keyword forms and values.

```ebnf
program ::= statement { statement }
statement ::= expression
variable-declaration ::= "SET" identifier "<" type ">" "=" expression
                       | "SET_CONST" identifier "<" type ">" "=" expression
variable-assignment ::= "SET" identifier "=" expression
variable-read ::= "GET" identifier
```

`statement` is parsed by `vltc::Parser::get_module_node`, which calls `vltc::Parser::get_expretion`. The declaration and assignment alternatives are parsed by `vltc::Parser::get_set_variable_node`; `variable-read` is parsed by `vltc::Parser::get_get_variable_node`.

An expression by itself is accepted as a program statement. The parser has no separate statement parser: module and block bodies call `vltc::Parser::get_expretion`, and `vltc::Parser::get_primary` dispatches declarations, assignments, function definitions, returns, conditionals, loops, and loop-control forms. These constructs are therefore syntactically accepted wherever a primary expression is expected, even where their result is semantically incompatible. For example, this parses, then fails analysis with `TypeError: expected 'int', got 'void'`:

```text
SET a<int> = IF True THEN 1 END
```

`SET name<type> = value` declares a typed variable; `SET name = value` assigns to an existing variable. `SET_CONST` requires an explicit type.

## 4. Functions

`vltc::Parser::get_user_function_node` parses definitions and parameters. `vltc::Parser::get_functioncall_node` parses calls. `vltc::Parser::get_return_noode` parses return forms.

```ebnf
function-definition ::= "FUNCTION" identifier "(" { parameter } ")" "-" ">" type { statement } "END"
parameter ::= identifier "<" type ">" [ "=" expression ]
function-call ::= "CALL" identifier { call-argument } "END"
call-argument ::= identifier "<" type ">" ":" expression
return-statement ::= "RETURN" expression
```

Parameters and call arguments are repeated without commas. A call argument has a name, a type tag, and an expression. The return-type arrow is two lexer tokens, `-` and `>`; whitespace may occur between them because the lexer discards whitespace.

The parser has a special branch for the token text `void` after `RETURN`, but that branch does not consume it. Running a non-void function containing `RETURN void` produces `SyntaxError: unexpected token 'void'`. This branch is in `vltc::Parser::get_return_noode` (`src/Grammars/UserFunction.cpp`).

## 5. Control flow

`vltc::Parser::get_if_statment_node`, `vltc::Parser::get_while_loop_node`, `vltc::Parser::get_for_loop_node`, `vltc::Parser::get_break_loop_node`, and `vltc::Parser::get_continue_loop_node` implement these forms.

```ebnf
if-statement ::= "IF" expression "THEN" { statement } [ "ELSE" "THEN" { statement } ] "END"
while-statement ::= "WHILE" expression "THEN" { statement } "END"
for-statement ::= "FOR" expression ";" expression ";" expression "THEN" { statement } "END"
break-statement ::= "BREAK"
continue-statement ::= "CONTINUE"
```

The `FOR` initializer, condition, and update are each parsed by `vltc::Parser::get_expretion`; the initializer is not a separate header grammar. Since `vltc::Parser::get_primary` accepts `SET`, a declaration such as `SET i<int> = 0` can be the first header expression. All three expressions and both semicolons are required.

An `IF` else branch uses the two keywords `ELSE THEN`. There is no `ELSE IF` or `ELIF` production. `BREAK` and `CONTINUE` are accepted without a parser or semantic loop-context check. Current behavior for use outside a loop is described in [Known limitations](limitations.md#known-limitations) and may change in a later version. Inside loops, the loop nodes handle these states.

## 6. Expressions and precedence

The following productions are ordered from lowest to highest precedence. The parser methods are in `src/Grammars/Main.cpp`.

```ebnf
expression ::= logical-or
logical-or ::= logical-and { "||" logical-and }
logical-and ::= equality { "&&" equality }
equality ::= comparison [ ( "==" | "!=" ) comparison ]
comparison ::= additive [ ( "<" | ">" | "<=" | ">=" ) additive ]
additive ::= multiplicative { ( "+" | "-" ) multiplicative }
multiplicative ::= unary { ( "*" | "/" | "%%" | "%" ) unary }
unary ::= ( "+" | "-" | "!" ) unary | power
power ::= primary [ "**" unary ]
primary ::= string | number | boolean | variable-read | function-call
          | variable-declaration | variable-assignment | function-definition
          | return-statement | if-statement | while-statement | for-statement
          | break-statement | continue-statement | "(" expression ")"
```

`expression`, `logical-or`, `logical-and`, `equality`, `comparison`, `additive`, `multiplicative`, `unary`, `power`, and `primary` are implemented by `vltc::Parser::get_expretion`, `vltc::Parser::get_logical_or`, `vltc::Parser::get_logical_and`, `vltc::Parser::get_equality`, `vltc::Parser::get_comparison`, `vltc::Parser::get_additive`, `vltc::Parser::get_multiplicative`, `vltc::Parser::get_unary`, `vltc::Parser::get_power`, and `vltc::Parser::get_primary`, respectively. In `primary`, the specialized forms delegate to the parser methods cited in their sections above; literals delegate to `vltc::Parser::get_value_node`.

Logical, additive, multiplicative, unary, and power operators are parsed as left- or right-recursive chains as shown: `||`, `&&`, `+`, `-`, `*`, `/`, `%%`, and `%` group left to right; unary operators associate right to left; `**` associates right to left. Equality and relational levels accept at most one operator each, so chained comparisons such as `a < b < c` and `a == b == c` are rejected.

The right operand of `**` is `unary`, so it can start with a unary sign, and recursive power parsing makes `2 ** 3 ** 2` group as `2 ** (3 ** 2)`. Since unary parsing falls through to power, `-2 ** 2` groups as `-(2 ** 2)`. `GET` consumes one identifier as a primary; to apply operators to a retrieved value, use it as a primary in an expression, for example `(GET score) >= 0`.

Numeric literals follow the `number` rule in section 2; `12.` is accepted as a float.


## 7. Semantic constraints

These checks are not represented by EBNF:

- `SetVariableNode::accept` checks whether the initializer type is compatible with the declared type. `are_types_compatible` in `src/Globals.cpp` accepts identical types, `int`/`float` pairs, and pairs where either side is `any`.
- Variable and function references are resolved against the active scope. Duplicate declarations and incompatible assignments are checked by their AST nodes.
- Function parameter defaults are checked for compatibility by `UserFunctionArgumentNode::accept`. Function bodies check return expression compatibility in `UserFunctionNode::accept`.
- `IF`, `WHILE`, and `FOR` conditions are checked by their AST nodes; the loop and conditional nodes require a boolean condition.
- The analyzer does not ban the `void` type token in variable or parameter declarations. It still checks compatibility with the initializer or default: the tested `SET x<void> = 1` fails with `TypeError: expected 'void', got 'int'`.

## 8. Known limitations

- **`void` declarations:** `void` is a recognized type in declarations and parameters, but ordinary non-void initializers/defaults fail compatibility checking.
- **`RETURN void`:** tested in a non-void function and rejected with `SyntaxError: unexpected token 'void'`; `vltc::Parser::get_return_noode` recognizes the token text but does not consume it.
- **Loop control outside loops:** the top-level `BREAK` example was accepted and execution continued after it. The semantic analyzer does not check loop nesting, and `ModuleNode::exec` ignores `ExecState` for top-level statements.
- **Primary expressions:** `vltc::Parser::get_primary` treats statement-like forms as primaries. The parser may therefore accept constructs in expression positions that later fail semantic type checking.

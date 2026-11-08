**Version:** 1.0.0

# Lexical Structure

## 1. Overview

The lexer reads source text and returns Token values. Its implementation is Lexar in `src/Lexar.cpp`, declared in `include/detail/lexer.hpp`. `vltc::Lexar::get_the_next_token` scans one token at a time; `vltc::Lexar::get_all_tokens` collects tokens through the end-of-code token.

Lexar receives a `const std::string&` and copies it into its code member. When source comes from a file, vltc::Runtime::RunTime::run_file reads the file in binary mode and removes a UTF-8 byte-order mark at the start before passing the text to the lexer (`src/Runtime.cpp`). This section describes tokenization only; see [Grammar](grammar.md) for syntax after tokenization and [Errors](errors.md) for diagnostic categories.

## 2. Source text and encoding

The lexer indexes a std::string one char at a time. It does not decode UTF-8 or count Unicode code points. File input is read as bytes by vltc::Runtime::RunTime::run_file; a leading UTF-8 BOM is removed there, not by Lexar.

Identifiers use ASCII letters, digits, and underscore only. UTF-8 is not decoded: columns and token offsets count source bytes. A well-formed multi-byte UTF-8 sequence outside strings and comments is returned as one UNKNOWN token.

## 3. Whitespace and line tracking

`vltc::Lexar::escape_spaces` skips ASCII space, `\t`, `\n`, `\v`, `\f`, and `\r`. Line counting advances on `\n`; in CRLF input, `\r` is skipped and `\n` advances the line once.

Line and column counters start at zero. Columns count bytes, not Unicode code points or visual display width; a tab advances by one byte. `vltc::Lexar::advence` updates the line when leaving a newline, so the first byte on the next line has column zero.

## 4. Comments

Before scanning tokens, `vltc::Lexar::escape_comments` removes comments from the source text. The parser therefore receives no comment tokens or comment text.

| Form | Behavior |
|---|---|
| `//` | Removes the `//` marker and every following character on that line, up to the next `\n` or end of input. This applies whether the comment starts a line or follows code. |
| `/* ... */` | Removes the opening marker, everything through the first following `*/`, and the closing marker. Comments do not nest. |

If a block comment reaches end of input before */, the lexer emits one `UNTERMINATED_COMMENT` token with value `unterminated block comment`; the next token is END_CODE. This behavior is implemented by `vltc::Lexar::escape_comments` and `vltc::Lexar::get_the_next_token`.

## 5. Identifiers

The identifier scan is implemented in `vltc::Lexar::get_the_next_token`.

```ebnf
identifier ::= ( letter | "_" ) { letter | digit | "_" }
```

The first character must be an ASCII letter or `_`. Following characters may also be ASCII digits. Non-ASCII characters are not identifier characters; UTF-8 sequences outside strings and comments produce one `UNKNOWN` token per sequence.

## 6. Keywords

Exact matches against `__key_words__` in `src/Globals.cpp` become KEY_WORD tokens. Matching is case-sensitive because the lexer compares the scanned text with these exact strings.

| Keyword | Token type |
|---|---|
| CALL | KEY_WORD |
| SET | KEY_WORD |
| SET_CONST | KEY_WORD |
| GET | KEY_WORD |
| FUNCTION | KEY_WORD |
| RETURN | KEY_WORD |
| IF | KEY_WORD |
| ELSE | KEY_WORD |
| THEN | KEY_WORD |
| WHILE | KEY_WORD |
| FOR | KEY_WORD |
| BREAK | KEY_WORD |
| CONTINUE | KEY_WORD |

END is handled separately as END_BLOCK, not as KEY_WORD.

| Source text | Token type | Token role |
|---|---|---|
| END | END_BLOCK | End-of-block token |
| (end of source) | END_CODE | End-of-input token |

## 7. Type names

The exact strings in `__types__` in `src/Globals.cpp` become TYPE tokens in `vltc::Lexar::get_the_next_token`. They are not emitted as ordinary identifiers.

| Type spelling | Token type |
|---|---|
| str | TYPE |
| int | TYPE |
| float | TYPE |
| bool | TYPE |
| void | TYPE |
| any | TYPE |

Type-name matching is case-sensitive.

## 8. Literals

### Numbers

The number scanner in `vltc::Lexar::get_the_next_token` starts only when the current character satisfies `is_digit`.

```ebnf
number ::= digit { digit } [ "." { digit } ]
```

The scanner accepts at most one dot and does not require digits after it. A dot cannot start a number. Thus `1.` is FLOAT, while `1.5.3` is split into FLOAT(`1.5`) and UNKNOWN(`.`), then INTEGER(`3`). Scientific notation is not recognized. A leading + or - is emitted as a separate operator token; it is not part of the numeric token.

```text
1.    -> FLOAT("1.")
1.5.3 -> FLOAT("1.5") UNKNOWN(".") INTEGER("3")
.5    -> UNKNOWN(".") INTEGER("5")
1e3   -> INTEGER("1") IDENTIFIER("e3")
-12   -> MINUS("-") INTEGER("12")
```

### Strings

The lexer recognizes text beginning with either " or ' as a string. The matching quote ends the token. The token's value contains the string content, without the delimiters.

Supported escapes in `__backslashed_symbols__` in `src/Globals.cpp` are `\n`, `\t`, `\\`, `\"`, and `\'`; they insert the corresponding character into the string value. An unknown escape preserves both the backslash and the escaped byte as written. Strings cannot contain a literal line feed. A line feed or end of input before the closing quote emits an unterminated-string lexical error.

```text
"hi"        -> STRING("hi")
"say \"hi\"" -> STRING("say \"hi\"")
"\\"        -> STRING("\\")
"\q"        -> STRING("\q")
"abc        -> UNTERMINATED_STRING
```

### Booleans

Only the exact spellings True and False are emitted as BOOLEAN tokens. Matching is case-sensitive. Other capitalization, such as true or FALSE, follows the identifier path and becomes IDENTIFIER.

## 9. Operators and punctuation

`vltc::Lexar::get_the_next_token` checks the two-character symbol map `__complex_2_symbols__` before the single-character symbol map `__symbols__` in `src/Globals.cpp`. This gives the listed two-character spellings priority over their one-character prefixes. There are no three-character tokens.

### Two-character tokens

| Text | TokenType | Token role |
|---|---|---|
| == | EQUAL_EQUAL | Equality operator |
| != | NOT_EQUAL | Inequality operator |
| <= | LESS_EQUAL | Less-than-or-equal operator |
| >= | GREATER_EQUAL | Greater-than-or-equal operator |
| && | LOGICAL_AND | Logical AND operator |
| \|\| | LOGICAL_OR | Logical OR operator |
| %% | DIV | Integer-division operator token |
| `**` | POWER | Power operator |

### Single-character tokens

| Text | TokenType | Token role |
|---|---|---|
| : | COLON | Colon punctuation |
| < | LESS_THAN | Less-than operator |
| > | GREATER_THAN | Greater-than operator |
| [ | LEFT_BRACKET | Left square bracket |
| ] | RIGHT_BRACKET | Right square bracket |
| { | LEFT_BRACE | Left brace |
| } | RIGHT_BRACE | Right brace |
| ( | LEFT_PAREN | Left parenthesis |
| ) | RIGHT_PAREN | Right parenthesis |
| / | SLASH | Slash operator |
| U+005C | BACKSLASH | Backslash symbol |
| ; | SEMICOLON | Semicolon punctuation |
| , | COMMA | Comma punctuation |
| `*` | STAR | Multiplication operator |
| - | MINUS | Minus operator |
| + | PLUS | Plus operator |
| = | EQUAL | Assignment/equal symbol |
| ^ | CARET | Caret symbol |
| ! | EXCLAMATION | Exclamation mark |
| % | MOD | Modulo operator token |

The lexer has no combined -> token. It emits MINUS("-") followed by GREATER_THAN(">").

```text
a==b -> IDENTIFIER("a") EQUAL_EQUAL("==") IDENTIFIER("b")
a->b -> IDENTIFIER("a") MINUS("-") GREATER_THAN(">") IDENTIFIER("b")
```

## 10. Unknown characters

If a character is not whitespace, part of a comment, a recognized number/identifier/string, or present in either symbol map, `vltc::Lexar::get_the_next_token` returns UNKNOWN. A valid multi-byte UTF-8 sequence outside strings/comments is grouped into one UNKNOWN token; ASCII unknown characters such as @, #, and $ each form one token.

Unsupported characters emit `UNKNOWN` tokens and are reported as `SyntaxError: unexpected token '…'`. Unterminated strings and comments have dedicated token types and messages: `unterminated string literal` and `unterminated block comment`.

## 11. Token structure

Token is declared in `include/valtcommand/globals.hpp`.

| Field | Meaning |
|---|---|
| Type | The TokenType category. |
| value | Token text or value. For strings this is decoded content without quotes; for END_CODE it is empty. |
| line | Lexer line counter at token creation; starts at 0. |
| column | Lexer column counter at token creation; starts at 0 for the first source character. |
| last_column | An ending-column field; there is no separate token-length field. |

Tokens carry zero-based starting lines and byte columns and an inclusive `last_column`. `vltc::Token::print` prints the type, value, line, and starting column, but not `last_column`. `vltc::Errors::ErrorInfo` and `vltc::Runtime::RunResult` retain zero-based positions; the diagnostic formatter displays line numbers starting at one and aligns the caret with the byte column.

## 12. End of input

`vltc::TokenType::END_CODE` is the end-of-input token. Its value is empty; line, column, and last_column are set to the current lexer counters. `vltc::Lexar::get_all_tokens` always appends this token, including for an empty input string.

For empty input, the constructor sets code_ended immediately. The first call to get_the_next_token returns END_CODE at line 0, column 0.

## 13. Lexer errors

The lexer emits `UNKNOWN` tokens for unsupported characters and dedicated `UNTERMINATED_COMMENT` / `UNTERMINATED_STRING` tokens for unterminated comments and strings. The parser reports unterminated comments and strings with dedicated `SyntaxError` messages. See [Errors](errors.md) for the interpreter's error reporting.

## 14. Known limitations

- The lexer is byte-oriented and does not decode UTF-8; visual caret alignment for wide glyphs depends on the terminal.
- Strings cannot span a line; a missing closing quote is a lexical error.
- A leading dot is not part of a number, and exponent notation is not recognized.
- -> is two tokens, not one.


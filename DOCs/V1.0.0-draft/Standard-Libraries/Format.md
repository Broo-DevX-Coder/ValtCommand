# Format Function

The `format()` function is part of the ValtCommand embedded standard library. It creates formatted strings by replacing named placeholders with the values of the corresponding arguments.

## Syntax

```sql
CALL format
    text<str>:"Hello {name}!"
    name<str>:"World"
END
```

**Output value:**

```text
Hello World!
```

The `text` argument contains the format string. All other arguments are named values that can be referenced inside the string using placeholders.

## Arguments

| Argument | Type | Required | Description |
|---|---|---|---|
| `text` | `str` | Yes | The template string containing placeholders. |
| Named arguments | Any supported value type | When referenced | Values used to replace placeholders in the template. |

The `format()` function accepts dynamically named arguments. You do not need to declare each possible placeholder name in advance.

### The `text` argument

The `text` argument defines the string to format and must be a string value.

```sql
CALL format
    text<str>:"Welcome to ValtCommand!"
END
```

Result:

```text
Welcome to ValtCommand!
```

Templates without placeholders are valid.

### Named arguments

Named arguments provide values for the placeholders used in `text`.

```sql
CALL format
    text<str>:"Hello {name}, welcome to {language}!"
    name<str>:"Developer"
    language<str>:"ValtCommand"
END
```

Result:

```text
Hello Developer, welcome to ValtCommand!
```

Each placeholder must match the name of a supplied argument exactly.

## Supported Value Types

The function converts supported runtime values to strings before inserting them into the result.

| Type | Example value | Formatted text |
|---|---|---|
| `str` | `"ValtCommand"` | `ValtCommand` |
| `int` | `18` | `18` |
| `float` | `3.14` | `3.14` |
| `bool` | `true` | `true` |
| `bool` | `false` | `false` |

Example:

```sql
CALL format
    text<str>:"Name: {name}, Age: {age}, Active: {active}"
    name<str>:"Developer"
    age<int>:18
    active<bool>:true
END
```

Result:

```text
Name: Developer, Age: 18, Active: true
```

Floating-point output follows the runtime's numeric-to-string conversion.

## Using `format()` with Other Functions

The formatted string can be passed directly to another function, such as `print`.

```sql
CALL print
    value<str>: CALL format
        text<str>:"Hello {name}!"
        name<str>:"World"
    END
END
```

Output:

```text
Hello World!
```

This allows you to construct messages dynamically without concatenating strings manually.

## Multiple Placeholders

A template can contain multiple placeholders, including repeated references to the same argument.

```sql
CALL format
    text<str>:"{name} is using {language}. Welcome, {name}!"
    name<str>:"Developer"
    language<str>:"ValtCommand"
END
```

Result:

```text
Developer is using ValtCommand. Welcome, Developer!
```

Repeated placeholders reuse the same named argument value.

## Error Handling

The function returns a runtime error when the template or its arguments are invalid.

### Missing `text`

The `text` argument is mandatory.

```sql
CALL format
    name<str>:"World"
END
```

Error type: `ArgumentError`

The error indicates that the required `text` argument was not supplied.

### Invalid `text` type

The `text` argument must be a string.

```sql
CALL format
    text<int>:123
END
```

Error type: `TypeError`

### Missing placeholder argument

Every placeholder must reference an argument supplied to the function.

```sql
CALL format
    text<str>:"Hello {name}!"
END
```

Error type: `ArgumentError`

The error identifies the missing argument `name`.

### Unmatched opening brace

Every opening `{` used to start a placeholder must have a corresponding closing `}`.

```sql
CALL format
    text<str>:"Hello {name"
    name<str>:"World"
END
```

Error type: `FormatError`

### Empty placeholder

Placeholder names cannot be empty.

```sql
CALL format
    text<str>:"Hello {}!"
END
```

Error type: `FormatError`

## Placeholder Rules

- Placeholders use the `{name}` syntax.
- The name must match a supplied named argument exactly.
- Every referenced argument must exist.
- Empty placeholders are invalid.
- An opening brace without a closing brace is invalid.
- Arguments that are not referenced by the template are ignored.
- Braces are reserved for placeholder syntax; literal brace escaping is not supported by the current implementation.

## Limitations

The current implementation provides basic named-placeholder substitution. It does not support:

- Positional placeholders.
- Numeric formatting options, such as fixed decimal precision.
- Alignment, padding, or width specifiers.
- Conditional formatting.
- Escape sequences for literal braces.

For example, `{price:.2f}` is **not** a supported formatting specifier. The implementation treats the text inside the braces as an argument name.

## Summary

Use `format()` when you need to build a string from named values.

```sql
CALL format
    text<str>:"{greeting}, {name}! Version: {version}"
    greeting<str>:"Hello"
    name<str>:"Developer"
    version<str>:"1.0.0"
END
```

Result:

```text
Hello, Developer! Version: 1.0.0
```

Remember: **`text` defines the template, and named arguments provide the values used to replace its placeholders.**

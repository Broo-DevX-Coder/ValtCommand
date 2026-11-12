# ValtCommand CLI Reference

**Version:** 1.0.0

The command-line tool runs ValtCommand source code from a file or directly from the command line, optionally with the debugger enabled. The CLI is built with CLI11.

> Everything in this document is derived from `main.cpp`. Items marked **(unverified)** depend on CLI11 or Runtime behavior that `main.cpp` does not show; test them before publishing.

---

## 1. Synopsis

```
valtc [OPTIONS]
```

Exactly one source must be given: a file (`--file`) or code (`--eval`).

```
valtc --file main.vlt
valtc --eval "CALL print \"Hello World!\" END"
valtc --debug --break main.vlt:1,2,3 --file main.vlt
```

---

## 2. Options

| Option | Aliases | Value | Default | Needs `--debug` |
|---|---|---|---|---|
| `-f` | `--file` | `FILE` | none | no |
| `-e` | `--eval` | `CODE` | none | no |
| `-d` | `--debug`, `--no-debug` | none (flag) | off | - |
| `-l` | `--logs`, `--no-logs` | none (flag) | off | **yes** |
| | `--log-file`, `--logsf` | `FILE` | none | **yes** |
| | `--break` | `BREAKPOINT...` | none | **yes** |
| `-v` | `--version` | none | | no |
| `-h` | `--help` | none | | no |

### `-f, --file FILE`
Path of the source file to run.
- The path must exist and must be a regular file (not a directory).
- Relative paths are resolved from the current working directory.
- It is the default file for `--break` entries written with only line numbers.
- Cannot be combined with `--eval`.

### `-e, --eval CODE`
Source code to run directly, without a file.
- Must be non-empty.
- Cannot be combined with `--file`.
- Errors from eval code have no file name, so no `File:` line is printed (see section 6).

### `-d, --debug` / `--no-debug`
Turns on the runtime's debug mode. `--no-debug` turns it off explicitly (it is already off by default).
- Required for `--break`, `--logs` and `--log-file` to have any effect.
- This flag is passed to the runtime when it is created.

### `-l, --logs` / `--no-logs`
Prints debugger log messages to the **standard error** stream (`std::clog`).
- Only works with `--debug`.
- Ignored when `--log-file` points to a valid file (the file wins, nothing goes to the console).

### `--log-file, --logsf FILE`
Writes debugger log messages to `FILE`.
- Only works with `--debug`.
- The file is created, or overwritten if it already exists.
- Takes priority over `--logs`.
- If `FILE` is an existing directory, the option is ignored and `--logs` rules apply.
- If the file cannot be opened (for example no permission), logs are silently lost and no error is shown. **(unverified: depends on stream state handling in Runtime)**

### `--break BREAKPOINT...`
Registers breakpoints. Accepts one or more values.

Each value has one of two forms:

| Form | Meaning |
|---|---|
| `PATH:LINE[,LINE...]` | Lines in the given file |
| `LINE[,LINE...]` | Lines in the file given by `--file` |

Rules:
- Line numbers start at **1**.
- Only digits are allowed (no signs, no spaces).
- The path is separated from the lines at the **last** `:`, so paths that contain `:` (for example `C:\dir\a.vlt:5`) work.
- Repeating the same line for the same path is allowed and is registered once.
- A trailing comma (`1,2,`) is accepted.
- Only works with `--debug`. Without it, the values are ignored without any message.
- Breakpoint paths are compared after conversion to absolute, lexically normalized paths. The tested spellings `main.vlt`, `./main.vlt`, and the absolute path therefore match each other when they identify the same file.
- CLI breakpoints remain active after a hit. A breakpoint on a loop-body line stops each time execution reaches that line after another line; in the documented two-iteration probe it stopped twice.
- With `--eval`, a line-only breakpoint has an empty file path, which matches any event file. `--break 1` was tested and stops on line 1; `--break :1` is rejected as an empty file path.
- The debugger checks breakpoints on `Runtime` events when either the line or file differs from the last Runtime event, then matches both file and line. Several nodes on one source line cause one stop; a later visit after another line can stop again. See [Debugger and tracer events](Language-references/tracer-debugger.md) for details.

Both forms below are supported:

```
valtc --debug --break main.vlt:1,2,3 utils.vlt:5,10 --file main.vlt
valtc --debug --break=main.vlt:1,2,3 utils.vlt:5,10 --file main.vlt   (unverified)
```

### `-v, --version`
Prints the version and license text, then exits with code `0`.

### `-h, --help`
Prints the help text and exits with code `0`.

---

## 3. How the source is chosen

The program checks in this order:

1. If `--file` points to an existing regular file: run it.
2. Otherwise, if `--eval` is non-empty: run the code.
3. Otherwise: print `No work to do ...` to **standard output** and exit with code `2`.

Consequences:
- Running with no arguments prints `No work to do ...` to standard output and exits with code `2`; it does not print help.
- A wrong file path (`--file missing.vlt`) also prints `No work to do ...` and exits with code `2`, not "file not found".
- Both cases exit with code `2`; these outputs and exit codes were tested with `valtc`.
- Giving both `--file` and `--eval` is rejected by the parser before anything runs (see section 7).

---

## 4. Debug and log routing

Everything below applies only when `--debug` is on.

| `--log-file` | `--logs` | Where debugger logs go |
|---|---|---|
| valid file | any | the file |
| not given, or a directory | on | standard error |
| not given, or a directory | off | nowhere (discarded) |

When `--debug` is off, none of these options do anything.

---

## 5. Output streams

| Stream | Content |
|---|---|
| Standard output | `No work to do ...`, `--help`, `--version`, and anything the script itself prints **(unverified: depends on the script's output library)** |
| Standard error | Script errors, breakpoint syntax errors, debugger logs (with `--logs`), parser errors from CLI11 |

---

## 6. Script error output

When the script fails (a parse error or a runtime error), the CLI prints to **standard error**:

1. an empty line,
2. the formatted error from `Errors::formatter` (type, message, separator line, numbered source line),
3. the line `File: "<absolute path>"`, only if the error belongs to a file,
4. an empty line.

Example:

```
TypeError: expected 'int', got 'str'
------------------------------------------
1-|  SET i<int> = "55"

File: "/home/user/project/tests/n.vlt"
```

Details:
- The source line shown is read again from the file named in the error. If that file cannot be read, the source part is empty.
- The file path is the absolute, normalized path and is printed with quotes.
- For `--eval`, the shown source is the eval text and no `File:` line is printed.
- The error can belong to a different file than the main file, and the file shown is always the one where the error happened.
- The exit code is `1`.

---

## 6.1 Breakpoint syntax errors

Printed to standard error as one line. The program exits with code `2` **before** running any code. `<p>` is the whole value as typed.

| Message | Cause | Example |
|---|---|---|
| `Empty file path in breakpoint: <p>` | Nothing before the `:` | `:5` |
| `Invalid breakpoint: <p>` | A line item is empty or not only digits | `main.vlt:a`, `main.vlt:1,,2`, `main.vlt:-3` |
| `Line number is too big: <p>` | The number does not fit in `size_t` | `main.vlt:99999999999999999999999` |
| `Lines start from 1: <p>` | A line is `0` | `main.vlt:0` |
| `No line numbers in breakpoint: <p>` | Nothing after the `:` | `main.vlt:` |

Breakpoint values are only checked when `--debug` is on.

---

## 7. Exit codes

| Code | Meaning |
|---|---|
| `0` | The script finished without error, or `--help` / `--version` was used |
| `1` | The script failed with a parse or runtime error |
| `2` | Nothing to run (`No work to do ...`), or an invalid breakpoint |
| `108` | CLI11 rejected the excluded `--file` and `--eval` option pair |
| `109` | CLI11 received an unknown option |
| `114` | CLI11 found the required `--file` value missing |

For these parser errors, CLI11 prints its message and help hint. The listed parser codes were verified by running `valtc --unknown`, `valtc --file` and `valtc --file Code/tests/n.vlt --eval x`.

A script that ends the program itself (for example through an exit call inside the script) may produce a different code. **(unverified)**

---

## 8. Examples

Run a file:
```
valtc --file main.vlt
```

Run code directly:
```
valtc --eval "CALL print \"Hello World!\" END"
```

Debug with breakpoints in two files, logs to the console:
```
valtc --debug --logs --break main.vlt:1,2,3 utils.vlt:5,10 --file main.vlt
```

Write debug logs to a file:
```
valtc --debug --file main.vlt --logsf runtime.log
```

Use the exit code in a shell script:
```
valtc --file build.vlt
if [ $? -ne 0 ]; then echo "script failed"; fi
```

---

## 9. Quick troubleshooting

| Symptom | Reason |
|---|---|
| `No work to do ...` although you passed `--file` | The path does not exist or is not a regular file |
| Breakpoints never trigger | `--debug` is missing, or the path/line does not match the loaded file |
| `--logs` prints nothing | `--debug` is missing, or `--log-file` is set to a valid file |
| Log file is empty or missing | `--debug` is missing, or the file could not be created |
| No `File:` line in the error | The code was run with `--eval` |

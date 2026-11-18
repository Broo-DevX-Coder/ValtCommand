#!/usr/bin/env python3
import difflib
import os
import re
import subprocess
import sys
from decimal import Context
from pathlib import Path

_FLOAT = re.compile(r"-?\d+\.\d+(?:[eE][+-]?\d+)?|-?\d+[eE][+-]?\d+")


def _cut(match) -> str:
    # Round one float token to 6 significant digits, formatted like C++ default / %g.
    d = Context(prec=6).create_decimal(match.group(0))
    if d == 0:
        return "0"
    sign, digits, exp = d.as_tuple()
    digits = "".join(map(str, digits))
    x = exp + len(digits) - 1
    s = digits.rstrip("0") or "0"
    if x < -4 or x >= 6:
        body = s[0] + ("." + s[1:] if len(s) > 1 else "") + f"e{'-' if x < 0 else '+'}{abs(x):02d}"
    elif x >= 0:
        body = s[:x + 1].ljust(x + 1, "0") + ("." + s[x + 1:] if len(s) > x + 1 else "")
    else:
        body = "0." + "0" * (-x - 1) + s
    return ("-" if sign else "") + body


def cut_floats(text: str) -> str:
    return _FLOAT.sub(_cut, text)


def main() -> None:
    root = Path(__file__).resolve().parent
    examples = Path(
        sys.argv[1]
        if len(sys.argv) > 1 and sys.argv[1]
        else root / "../../DOCs/V1.0.0/Language-references/examples"
    )
    interpreter = (
        sys.argv[2]
        if len(sys.argv) > 2 and sys.argv[2]
        else str(root / "../../Code/build/bin/valtc")
    )
    mode = (
        sys.argv[3]
        if len(sys.argv) > 3 and sys.argv[3]
        else "--preview"
    )

    if not os.access(interpreter, os.X_OK) or os.path.isdir(interpreter):
        print(f"Interpreter is not executable: {interpreter}", file=sys.stderr)
        sys.exit(2)

    if mode not in ("--preview", "--write"):
        raise SystemExit("mode must be --preview or --write")

    error = re.compile(
        r"(?m)^\s*(?:SyntaxError|NameError|TypeError|ArgumentError|FunctionError|"
        r"ReturnTypeError|MissingReturnError|ImmutableError|VariableError|"
        r"IncompatibleTypesError|RuntimeError|ValueError|OverflowError):"
    )
    changed = 0
    for source in sorted(examples.glob("*.vlt")):
        run = subprocess.run(
            [interpreter, "--file", str(source)],
            capture_output=True,
            text=True,
            env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"},
            check=False,
        )
        prefix = "ERROR" if run.returncode != 0 or error.search(run.stdout) else "OK"
        # Float numbers are cut to 6 significant digits only for successful runs,
        # so source lines echoed inside error messages stay untouched.
        stdout = cut_floats(run.stdout) if prefix == "OK" else run.stdout
        actual = prefix + "\n" + stdout + run.stderr
        expected = examples.parent / "exceptions" / source.with_suffix(".expected").name
        # Keep expected output portable while preserving the runtime's source path.
        actual = actual.replace(str(source.resolve()), f"<exemples_path>/{source.name}")
        old = expected.read_text(encoding="utf-8") if expected.exists() else ""
        if old == actual:
            continue
        changed += 1
        print(f"--- {expected}")
        print(f"+++ {'(generated)' if not expected.exists() else expected}")
        before = old.splitlines(keepends=True)
        after = actual.splitlines(keepends=True)
        diff = difflib.unified_diff(before, after, fromfile="before", tofile="after")
        print("".join(diff), end="" if actual.endswith("\n") else "\n")
        if mode == "--write":
            expected.write_text(actual, encoding="utf-8")
    print(f"{changed} expected file(s) differ; mode={mode}")


if __name__ == "__main__":
    main()
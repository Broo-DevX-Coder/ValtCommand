#!/usr/bin/env python3
import difflib
import os
import re
import subprocess
import sys
from pathlib import Path


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
        actual = prefix + "\n" + run.stdout + run.stderr
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

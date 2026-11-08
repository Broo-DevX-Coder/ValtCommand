#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
examples="${1:-$root/DOCs/V1.0.0/Language-references/examples}"
interpreter="${2:-$root/Code/build/bin/ValtCommand}"
mode="${3:---preview}"

if [[ ! -x "$interpreter" ]]; then
  printf 'Interpreter is not executable: %s\n' "$interpreter" >&2
  exit 2
fi

python3 - "$examples" "$interpreter" "$mode" <<'PY'
from pathlib import Path
import difflib
import os
import re
import subprocess
import sys

examples = Path(sys.argv[1])
interpreter = sys.argv[2]
mode = sys.argv[3]
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
        [interpreter, str(source)], capture_output=True, text=True,
        env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, check=False,
    )
    prefix = "ERROR" if run.returncode != 0 or error.search(run.stdout) else "OK"
    actual = prefix + "\n" + run.stdout + run.stderr
    expected = source.with_suffix(".expected")
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
PY

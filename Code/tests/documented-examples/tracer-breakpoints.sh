#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
interpreter="${1:-$root/Code/build/bin/valtc}"
source="$root/Code/tests/documented-examples/tracer-breakpoints.vlt"

if [[ ! -x "$interpreter" ]]; then
  printf 'Interpreter is not executable: %s\n' "$interpreter" >&2
  exit 2
fi

for line in 4 5 7 8 9 10 13 16 18; do
  output="$(printf 'continue\ncontinue\ncontinue\ncontinue\ncontinue\n' | \
    ASAN_OPTIONS=detect_leaks=0 "$interpreter" --debug --break "$source:$line" --file "$source" 2>&1)"
  prompts="$(printf '%s' "$output" | grep -o '> ' | wc -l)"
  if [[ "$prompts" -lt 1 ]]; then
    printf 'Breakpoint did not stop at line %s\n%s\n' "$line" "$output" >&2
    exit 1
  fi
  if [[ "$line" == 4 && "$prompts" -ne 1 ]]; then
    printf 'Line 4 should stop once despite multiple AST nodes; got %s stops\n' "$prompts" >&2
    exit 1
  fi
  if [[ "$line" == 16 && "$prompts" -ne 2 ]]; then
    printf 'Loop-body breakpoint should stop once per iteration; got %s stops\n' "$prompts" >&2
    exit 1
  fi
  printf 'line %s: %s stop(s)\n' "$line" "$prompts"
done

output="$(printf 'continue\ncontinue\n' | \
  ASAN_OPTIONS=detect_leaks=0 "$interpreter" --debug \
    --break "$source:4" "$source:4" --file "$source" 2>&1)"
prompts="$(printf '%s' "$output" | grep -o '> ' | wc -l)"
if [[ "$prompts" -ne 1 ]]; then
  printf 'Duplicate line 4 breakpoints should register once; got %s stops\n' "$prompts" >&2
  exit 1
fi
printf 'duplicate line 4: %s stop(s)\n' "$prompts"

temporary="$(mktemp -d)"
trap 'rm -rf "$temporary"' EXIT
g++ -std=c++20 -fsanitize=address,undefined -fno-omit-frame-pointer \
  -I"$root/Code/include" \
  "$root/Code/tests/documented-examples/tracer-same-line-files.cpp" \
  -L"$root/Code/build/bin" -Wl,-rpath,"$root/Code/build/bin" -lengine \
  -o "$temporary/tracer-same-line-files"
ASAN_OPTIONS=detect_leaks=0 "$temporary/tracer-same-line-files" \
  "$temporary/source"

# Performance Report: ValtCommand (Version 0.1)

## Summary

- All four benchmarks produce **correct results**.
- The slowest benchmark (10M-iteration loop) dropped from **76.9 s** to **~27 s** after only two fixes (**≈ 2.8× speedup**).
- Memory usage is **constant (~4.9 MB)** in every benchmark, with no sign of memory leaks.
- Current bottleneck: **frequent memory allocation** (`malloc`/`free`) caused by cloning AST nodes (`clone`) on every loop iteration.

## Measurement Environment

| Item | Value |
|---|---|
| Interpreter | ValtCommand (tree-walking) |
| Tool | `/usr/bin/time` |
| Runs per benchmark | 2 |
| Output | Redirected to `/dev/null` (no printing overhead) |
| Build type | To be confirmed (Release `-O2`) and recorded here |

## Benchmarks

| # | Benchmark | Size | Output | Correct? |
|---|---|---|---|---|
| 1 | `bench1_loop` | 10,000,000-iteration loop (summation) | `49999995000000` | ✅ |
| 2 | `bench2_fib` | `fib(30)` via loop × 100,000 calls | `832040` | ✅ |
| 3 | `bench3_call` | 1,000,000 calls to an `add` function | `499999500000` | ✅ |
| 4 | `bench4_nested` | Nested loops 3000 × 3000 | `9000000` | ✅ |

## Results

| Benchmark | Run 1 | Run 2 | Average | Time per unit |
|---|---|---|---|---|
| `bench1_loop` | 27.38 s | 26.82 s | **27.10 s** | ≈ 2.7 µs / iteration |
| `bench2_fib` | 11.95 s | 11.59 s | **11.77 s** | ≈ 118 µs / `fib(30)` call (≈ 3.8 µs / inner iteration) |
| `bench3_call` | 6.28 s | 6.55 s | **6.42 s** | ≈ 6.4 µs / (iteration + function call) |
| `bench4_nested` | 26.86 s | 26.87 s | **26.87 s** | ≈ 3.0 µs / inner iteration |

**Notes:**

- The difference between the two runs is small (under 3%), so the results are stable.
- `user ≈ elapsed` and `sys ≈ 0`: all the time is spent on the CPU, with no I/O.
- Peak memory is `~4.9 MB`, and page faults are `~220-240` in every benchmark.
- The cost of calling a simple function is estimated at roughly **3-4 µs** (the difference between `bench3` and `bench1`). This is a rough estimate.

## Before/After the Fixes (`bench1_loop`)

| State | Time |
|---|---|
| Before the fixes | 76.9 s |
| After the fixes | ~27.1 s |
| Improvement | ≈ 2.8× (≈ 65% reduction) |

**The two fixes applied:**

1. **`Scope`**: replaced `Utils::random_num()` with a counter (`next_id++`). `mt19937_64` and `random_device` were the biggest time consumers in `perf`.
2. **`emit_event`**: changed the parameter from `const std::string&` to `const char*`, so no temporary string is created unless a Debugger is attached.

> Benchmarks 2, 3 and 4 were measured **after** the fixes only, so there are no "before" numbers for them.

## `perf` Analysis (After the Fixes)

| Category | Functions seen | Approx. share |
|---|---|---|
| Memory allocation/deallocation | `malloc` 12.4%, `_int_free` 11.5%, `cfree` 5.7%, `operator new` 3.7% | **≈ 33%** |
| String operations (`std::string`) | `_M_mutate` 5.6%, `operator+` 3.7%, `_M_replace` 2.0%, `string` constructor 1.8% | **≈ 13%** |
| Node execution | `ResetVariableNode::exec` 10.5% (inclusive), `BinOpsNode::exec` 5.4%, `GetVariableNode::exec` 3.6% | ≈ 20% |
| Destroying cloned nodes | `~NumberValueNode` 4.5%, `~CompOpsNode` 3.4% | ≈ 8% |
| Variable lookup | `Scope::search_var` 4.9%, `_Hash_bytes` 2.4% | ≈ 7% |
| Number conversion | `turn_value_to_float` 2.5%, `strtold` 1.8% | ≈ 4% |

**Notes:**

- The percentages come from a single `perf` report that does not say which benchmark it profiled. Inclusive percentages also overlap, so they should not be added up literally.
- `[unknown]` entries are caused by missing frame pointers; they are not real functions.
- `random_num` has **disappeared** from the list, so the first fix worked.
- The appearance of `strtold` and `turn_value_to_float` suggests that numbers are converted from/to strings at runtime. This is an inference from the names and needs a code review.

## Assessment

- **Acceptable for a first version**, because the causes are known and documented.
- The numbers are slower than an optimized tree-walking interpreter (expected 1-3 s for 10M iterations). The main reason is architectural: cloning the AST on every iteration.

## Deferred Improvements (Next Version)

1. Remove `clone()` from loops (`Condition`, `Update`, `Statements`).
2. Remove `Statments.clear()` from the end of `exec`, so that execution no longer consumes the node.
3. Use a single `Scope` per loop instead of one per iteration (cleaning up its variables).
4. Reduce `std::string` copies for variable names (use an index or `string_view` instead of a string).
5. Store numeric values as ready-made numbers instead of converting them from strings at runtime.
6. `get_variables_table()` and `get_functions_table()` copy everything: make sure they are not called during execution.
7. Pass `NodesContext` by reference if it contains a `std::string FilePath`.
8. Re-measure with 5 runs and build with `-O2 -g -fno-omit-frame-pointer` for a clearer `perf` output.

## Conclusion

| Indicator | Status |
|---|---|
| Correctness | ✅ All results are correct |
| Stability | ✅ Variation under 3% |
| Memory | ✅ Constant (~4.9 MB) |
| Speed | ⚠️ Slow, with a known cause (`clone` + `malloc`) |
| Decision | ✅ **Freeze these numbers as the baseline and move on to the docs and API** |

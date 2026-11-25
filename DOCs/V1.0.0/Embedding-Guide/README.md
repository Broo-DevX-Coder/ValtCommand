# ValtCommand Embedding Guide

For C++ hosts using ValtCommand 1.0.0. See [installation](installation.md), [running code](running-code.md), [results](reading-results.md), [native functions](native-functions.md), [debugging and events](debugger-tracer-events.md), [limitations](known-limitations.md), the [RunTime reference](runtime.md), [Scope reference](scope.md), [Debugger reference](debugger.md), [reference index](runtime-reference.md), and [API coverage](API-Coverage.md).

Quick start:

```cpp
vltc::Runtime::RunTime runtime;
auto result = runtime.run("SET answer<int> = 2 + 3");
if (result) std::cout << runtime.get_variable("answer").value << "\n";
```

A complete package consumer and CTest setup is in [examples](examples/). `runtime.reset()` removes script-created variables and functions while preserving registered native functions and constants; see [running code](running-code.md).

# Public header API coverage

Inventory source: headers under `Code/include/valtcommand/`. Rows below enumerate public members of `RunTime`, `Scope`, and `Debugger` individually; other exported declarations are grouped by type. Private members and implementation helpers are excluded.

| Symbol / member | Definition | Documentation and section | Notes |
|---|---|---|---|
| `RunTime` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Runtime class. |
| `RunTime(bool DebugMode = false)` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Creates initialized runtime; debug flag controls debugger creation. |
| `RunTime(const std::string& code, bool DebugMode = false)` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Compatibility constructor with initial source. |
| `~RunTime()` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Destroys runtime-owned scopes and debugger. |
| `Initialyze(bool DebugMode = false)` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Recreates scopes and built-ins; can discard script state. |
| `GetDebugger()` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Returns owned debugger or null. |
| `set_debugger_ostream(std::ostream&)` | `runtime.hpp` | [runtime.md](./runtime.md) — Debugger stream access | Changes debugger output stream. |
| `get_debugger_ostream() const` | `runtime.hpp` | [runtime.md](./runtime.md) — Debugger stream access | Returns non-owning stream pointer or null. |
| `RunTime(const RunTime&) = delete` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Copy is disabled. |
| `operator=(const RunTime&) = delete` | `runtime.hpp` | [runtime.md](./runtime.md) — Construction and lifetime | Copy assignment is disabled. |
| `run(const std::string&, ScopeMode = Persistent)` | `runtime.hpp` | [runtime.md](./runtime.md) — Execution | Runs string source. |
| `run_file(const std::string&, ScopeMode = Persistent)` | `runtime.hpp` | [runtime.md](./runtime.md) — Execution | Runs source from file. |
| `reset()` | `runtime.hpp` | [runtime.md](./runtime.md) — State management | Clears script variables/functions and preserves registered native functions/constants. |
| `set_variable(const std::string&, const std::string&, const std::string&, bool = false)` | `runtime.hpp` | [runtime.md](./runtime.md) — State management | Writes typed global; native constant names are rejected. |
| `get_variable(const std::string&)` | `runtime.hpp` | [runtime.md](./runtime.md) — State management | Reads a global or returns host error. |
| `has_variable(const std::string&)` | `runtime.hpp` | [runtime.md](./runtime.md) — State management | Tests for a global variable. |
| `add_external_function(ExternalFuncType, const std::string&, const std::string&, unordered_map<...>, bool = false)` | `runtime.hpp` | [runtime.md](./runtime.md) — Native registrations | Registers callback; survives reset/Fresh. |
| `add_external_constant(const std::string&, const Value&)` | `runtime.hpp` | [runtime.md](./runtime.md) — Native registrations | Registers typed immutable constant; survives reset/Fresh. |
| `Scope` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Scope class. |
| `SymbolTableTypes` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Namespace for scope record types. |
| `VarsTableLine`, `FuncsTableLine` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Visible table row types. |
| `Scope(Scope* parent = nullptr)` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Creates scope with non-owning parent. |
| `Scope(const Scope&) = delete` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Copy constructor disabled. |
| `operator=(const Scope&) = delete` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Copy assignment disabled. |
| `Scope(Scope&&) noexcept` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Defaulted move constructor. |
| `operator=(Scope&&) noexcept` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Defaulted move assignment. |
| `get_id()` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Returns generated id. |
| `get_parent()` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Returns parent pointer. |
| `get_depth()` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Returns nesting depth. |
| `get_functions_table()` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Builds visible function rows. |
| `get_variables_table()` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Builds visible variable rows. |
| `add_function(const std::string&, const std::string&, unordered_map<...>, bool = false)` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Adds/replaces function; returned entry pointer may be invalidated. |
| `add_var(const std::string&, const std::string&, const Value&, bool = false)` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Adds/replaces variable; returned entry pointer may be invalidated. |
| `search_function(Token&)` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Looks up function in parent chain. |
| `search_var(Token&)` | `scope.hpp` | [scope.md](./scope.md) — Scope members | Looks up variable in parent chain. |
| `VarsTableLine::name` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Visible variable name. |
| `VarsTableLine::var_ptr` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Non-owning mutable variable entry pointer. |
| `FuncsTableLine::name` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Visible function name. |
| `FuncsTableLine::func_ptr` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Non-owning mutable function entry pointer. |
| `vars_table` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Vector alias for variable table rows. |
| `funcs_table` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Vector alias for function table rows. |
| `FunctionsTypes` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Function-origin enum. |
| `FunctionsTypes::Extenal` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Marks external C++ function. |
| `FunctionsTypes::Inside` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Marks script-defined function. |
| `SVar::type` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Mutable language type string. |
| `SVar::is_const` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Mutability metadata. |
| `RVar` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Runtime variable record. |
| `SVar` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Shared variable metadata. |
| `Argument` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Function argument metadata. |
| `Function` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Function record. |
| `RVar::value` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Mutable Value variant. |
| `RVar::scope_id` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Origin scope id. |
| `Argument::is_required` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Required argument flag. |
| `Argument::is_any` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Generic argument flag. |
| `Argument::default_value` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Default argument Value. |
| `Function::~Function()` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Destroys owned user AST pointer. |
| `Function::return_type` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Declared function return type. |
| `Function::arguments` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Argument metadata map. |
| `Function::is_sepport_any_arguments_` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Allows any arguments flag. |
| `Function::scope_id` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Defining scope id. |
| `Function::type` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Inside/external function tag. |
| `Function::external_func` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Native callback. |
| `Function::user_function` | `scope.hpp` | [scope.md](./scope.md) — Public records and aliases | Owned user AST pointer; mutation affects ownership. |
| `Debugger` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Debugger class. |
| `EventsT` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Event category enum. |
| `event` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Event struct. |
| `breakPointType` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Breakpoint record type. |
| `EventsT::Info` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | General information event. |
| `EventsT::Analysis` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Semantic analysis event. |
| `EventsT::Runtime` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Execution event. |
| `EventsT::Debug` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Debugger event. |
| `EventsT::Warning` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Warning event. |
| `EventsT::Error` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Failure event. |
| `event_type_to_string(EventsT)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Returns category name. |
| `event::type` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Event category. |
| `event::message` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Event message. |
| `event::line` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Source line. |
| `event::curent_scope` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Non-owning scope pointer. |
| `event::file` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Source path. |
| `event::get_str()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event types and records | Formats event text. |
| `breakPointType(size_t, const fs::path& = "", bool = false)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Constructs breakpoint metadata. |
| `breakPointType::set_av__(bool)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Enables/disables host-owned record. |
| `breakPointType::use()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Marks temporary record used. |
| `breakPointType::is_used() const` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Queries one-shot use. |
| `breakPointType::get_file() const` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Returns file path reference. |
| `breakPointType::operator bool() const` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Checks active state. |
| `breakPointType::operator size_t() const` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint record | Converts to line number. |
| `EmitSlotType` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Event callback signature. |
| `StopSlotType` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Stop callback signature. |
| `Debugger()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Default output is std::cout. |
| `Debugger(std::ostream&)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Uses non-owning stream. |
| `set_ostream(std::ostream&)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Replaces output stream. |
| `get_ostream() const` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Debugger construction and streams | Returns stream pointer. |
| `emit(event)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event tracking | Stores, dispatches, and may check runtime breakpoint. |
| `set_emit_event_slot(EmitSlotType)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event tracking | Replaces event callback. |
| `get_next_event_str()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event tracking | Returns cursor event then advances. |
| `get_all_events_strs()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Event tracking | Formats all events without moving cursor. |
| `add_break_points(const std::vector<size_t>&, const fs::path& = "")` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint control | Adds line/file breakpoints. |
| `check_break_point(size_t, Scope*, const fs::path& = "")` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint control | Matches breakpoints and calls stop callback. |
| `set_stop_slot(StopSlotType)` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint control | Replaces synchronous pause callback. |
| `continue_exec()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint control | Empty implementation in current source. |
| `next_line()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint control | Adds temporary next-line breakpoint. |
| `exit()` | `debugger/debugger.hpp` | [debugger.md](./debugger.md) — Breakpoint control | Calls std::exit(0), terminating host. |
| `VALTC_API` | `api_export.hpp` | [installation.md](./installation.md) — Consumer CMake | DLL visibility macro. |
| `Value` | `value.hpp` | [native-functions.md](./native-functions.md) — callback types | Runtime value alternatives. |
| `RunResult`, `RunError`, `ErrorStage`, `ScopeMode`, `Errors::ErrorInfo` | `result.hpp` | [running-code.md](./running-code.md) — failure results | Error payload and scope policy. |
| `ExternalFunctionArgument`, `ExternalFunInType`, `ExternalFuncType` | `native_function.hpp`, `globals.hpp` | [native-functions.md](./native-functions.md) — callback signature | Native callback contract. |
| `InspecturesReturn`, `ScopeInspect`, `VariablesInspect`, `FunctionsInspect` | `debugger/inspect.hpp` | [reading-results.md](./reading-results.md) — scope inspection | `JSON` member is serialized `std::string`. |
| `ExecState`, `ReturnResult<T>` | `globals.hpp` | [native-functions.md](./native-functions.md) — callback return | Callback result is used; interpreter control-flow fields are low-level. |
| `TokenType`, `Token` | `globals.hpp` | [scope.md](./scope.md) — lookup methods | Token is accepted by public `Scope::search_*`. |
| `vltc::fs` | `result.hpp` | API-Coverage.md — this inventory | Namespace alias. |
| `valtcommand.hpp` | `valtcommand.hpp` | [installation.md](./installation.md) — Consumer CMake | Umbrella include for installed API. |

## Moved out of the installed public headers

`include/detail/globals.hpp` contains `Errors::formatter`, `TokenTypesStr`, `TokenTypes_to_StringType`, `__types__`, `__key_words__`, `__symbols__`, `__complex_2_symbols__`, `__backslashed_symbols__`, `is_token_type_`, `is_token_key_word_`, `Get_ValueT`, `are_types_compatible`, `reconsiliation_int_float`, and `turn_value_to_float`. These implementation declarations are not installed as public host API. `TokenType` and `Token` remain public because `Scope::search_*` accepts `Token&`.

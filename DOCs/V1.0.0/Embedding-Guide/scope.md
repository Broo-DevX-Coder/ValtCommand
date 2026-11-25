# Scope reference

`vltc::Scopes::Scope` owns local variable and function entries and optionally points to a parent scope. Runtime-owned scopes are not directly exposed except through the pointer given to debugger/native callbacks. Public scope tables are built when queried; there is no cached visible-table state or `refresh()` member in the current header.

## Scope members

### `Scope(Scope* parent = nullptr)`

Creates a scope whose parent is non-owning. The parent must outlive this scope. Example: `Scope child(&parent);`. Destroying/replacing a parent while a child is in use leaves the child's parent pointer invalid.

### `size_t get_id()`

Returns this scope's generated identifier. Example: `auto id = scope.get_id();`. Read-only; it does not change execution.

### `Scope* get_parent()`

Returns the non-owning parent or null. Example: `auto* parent = scope.get_parent();`. Do not retain beyond the parent's lifetime.

### `size_t get_depth()`

Returns depth from the root (root depth is zero). Example: `scope.get_depth();`. Read-only.

### `funcs_table get_functions_table()`

Returns visible function names and raw entry pointers, including parent entries. The current parent-filtering code checks the local variable map, not the local function map: a same-named local variable can hide a parent function row, while a same-named local function can leave both parent and local function rows visible. Example: `auto visible = scope.get_functions_table();`. The vector is a snapshot, but each pointer refers to the scope-owned entry. Adding/replacing an entry while using its pointer can invalidate it. The implementation's parent filtering checks the local variable map for a matching function name; name collisions should be tested before relying on the result.

### `vars_table get_variables_table()`

Returns visible variable names and raw entry pointers from this scope and its parents, with local shadowing. Example: `auto visible = scope.get_variables_table();`. The returned vector is recomputed on each call; pointers remain owned by their scopes and can be invalidated by replacement or destruction.

### `SymbolTableTypes::Function* add_function(const std::string& name, const std::string& return_type, std::unordered_map<std::string, SymbolTableTypes::Argument> arguments, bool is_any = false)`

Adds or replaces a local function record and returns its pointer. Example: `scope.add_function("f", "int", {});`. Replacement destroys the prior record. `Function` destruction deletes `user_function`, so changing that pointer manually or replacing an entry during execution can invalidate active references and ownership.

### `SymbolTableTypes::RVar* add_var(const std::string& name, const std::string& type, const Value& value, bool is_const = false)`

Adds or replaces a local variable and returns its pointer. Example: `scope.add_var("n", "int", int64_t{4});`. Replacement destroys the previous record; any pointers to that record become invalid. The type string and `Value` alternative must agree for interpreter operations to behave correctly.

### `ReturnResult<SymbolTableTypes::Function*> search_function(Token& NameToken)`

Searches the local function map and then parents; returns success and a pointer, or error information when missing. Example: create an identifier `Token`, then call `scope.search_function(token)`. The returned pointer is non-owning; replacing its entry invalidates it.

### `ReturnResult<SymbolTableTypes::RVar*> search_var(Token& NameToken)`

Searches local variables and parents. Example: create an identifier `Token`, then call `scope.search_var(token)`. The returned pointer is non-owning; replacing its entry invalidates it.

## Public records and aliases

`VarsTableLine` exposes `name` and `RVar* var_ptr`; `FuncsTableLine` exposes `name` and `Function* func_ptr`. `vars_table` and `funcs_table` are vectors of those rows. `SVar` has mutable `type` and `is_const`; `RVar` adds mutable `Value value` and `scope_id`. `Argument` adds `is_required`, `is_any`, and `default_value`. `Function` exposes return type, argument map, any-argument flag, scope id, function kind, external callback, and user AST pointer. These are mutable low-level records. Changing an entry while execution is active can desynchronize type analysis from runtime values or invalidate active pointers; changing `Function::user_function` also changes an owned pointer deleted by the destructor. Use runtime host APIs for globals and avoid live mutation from callbacks unless the exact behavior has been tested.

No public `refresh()` method exists in the current `Scope` declaration. The visible tables are recomputed on each getter call. A proposed refresh behavior needs a defined purpose before it can be added or documented.

### `std::string SVar::type`

Stores the declared language type. Example: `entry->type`. Changing it during execution without changing `RVar::value` can make the type tag disagree with the active `Value` alternative and break analysis or evaluation.

### `bool SVar::is_const`

Stores constness metadata. Example: inspect `entry->is_const`. Changing it directly bypasses the language's declaration checks and may let script assignments violate intended const behavior.

### `Value RVar::value`

Stores the current value. Example: read with `std::get<int64_t>(entry->value)`. Replacing it during a callback can affect later reads, but the variant alternative must still match `type`; a mismatched pair can break operations and conversions.

### `size_t RVar::scope_id`

Records the defining scope id. Example: `entry->scope_id`. It is descriptive metadata; changing it does not move the owning entry and can make inspection misleading.

### `bool Argument::is_required`, `bool Argument::is_any`, `Value Argument::default_value`

These fields control required-argument validation, generic type acceptance, and default values. Example: read them from a function record's argument map. Changing them while a call is being analyzed or executed can make validated calls disagree with callback assumptions; keep the declared type and default variant consistent.

### `std::string Function::return_type`, `arguments`, `is_sepport_any_arguments_`, `scope_id`, `type`, `external_func`, and `user_function`

These public fields describe a function's signature, origin, and implementation. Example: inspect `function->return_type` from a table row. Mutating signature data during execution can invalidate semantic checks or argument maps. `external_func` changes which callback is invoked. `user_function` is deleted by `Function`'s destructor, so replacing or aliasing it manually can cause invalid ownership or double deletion. Avoid live mutation.

### `VarsTableLine::name`, `VarsTableLine::var_ptr`, `FuncsTableLine::name`, and `FuncsTableLine::func_ptr`

These fields identify a visible table row. Example: `row.name` and `row.var_ptr`. The name is a copy in the returned vector; the pointer is non-owning. Replacing or destroying the owning entry invalidates the pointer, while changing `name` only changes the local returned row.

### `FunctionsTypes::Extenal` and `FunctionsTypes::Inside`

These enum values label external and script-defined functions. Example: compare `function->type` with `FunctionsTypes::Extenal`. Changing the tag without updating the corresponding callback/AST pointer makes the record inconsistent and can break invocation or destruction.

### `vars_table` and `funcs_table`

These aliases are vectors of table rows. Example: `Scope::vars_table vars = scope.get_variables_table();`. The vector owns row copies, not the entries referenced by row pointers.

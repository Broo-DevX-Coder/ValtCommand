# Debugger reference

A debugger is created when `RunTime(true)` is used and obtained with `RunTime::GetDebugger()`. All debugger controls below are called on `DevTools::Debugger`; `RunTime` only provides debugger stream accessors. Debugger callbacks run synchronously during event emission or breakpoint checks.

## Event types and records

### `EventsT`

Values are `Info`, `Analysis`, `Runtime`, `Debug`, `Warning`, and `Error`. Example: `event.type == EventsT::Runtime`. This enum is read-only metadata; changing emitted events affects callback/log content, not the script's type system.

### `std::string event_type_to_string(EventsT tegory)`

Returns the category name as text. Example: `event_type_to_string(event.type)`. Read-only.

### `event` fields

The public members are `type`, `message`, `line`, `curent_scope`, and `file`. They contain the category, message, source line, non-owning current scope pointer, and source path. Example: `auto text = event.get_str();`. The scope pointer is valid only while its owning scope lives; retaining it beyond a callback/run can dangle. Mutating a received event only changes that event object/callback view.

### `std::string event::get_str()`

Formats an event as `[Type] line:N -> message` plus newline. Example: `out << event.get_str();`. It does not change the event or debugger state.

## Breakpoint record

### `breakPointType(size_t line, const fs::path& file = "", bool temporary = false)`

Creates a breakpoint record. Empty file matches any file in the check routine. Example: `breakPointType bp(12, "script.vlt");`. Records constructed separately do not enter a debugger's private breakpoint list unless passed through debugger APIs (which currently accept line numbers).

### `void set_av__(bool available)`

Enables or disables this record. Example: `bp.set_av__(false);`. The debugger's internal records are private, so this affects only a record the host owns.

### `void use()`

Marks a temporary record as used. Example: `bp.use();`. For a permanent record this leaves it unused.

### `bool is_used() const`

Reports whether the record has been used. Example: `if (bp.is_used()) { ... }`. Read-only.

### `const fs::path& get_file() const`

Returns the record's file path. Example: `auto path = bp.get_file();`. Returned reference belongs to the record.

### `explicit operator bool() const`

True when the record is available and not used. Example: `if (static_cast<bool>(bp)) { ... }`. Read-only.

### `explicit operator size_t() const`

Returns its line number. Example: `size_t line = static_cast<size_t>(bp);`. Read-only.

## Debugger construction and streams

### `Debugger()`

Uses `std::cout` for output. Example: `Debugger debugger;`. No runtime is attached automatically.

### `explicit Debugger(std::ostream& ostream)`

Uses the supplied stream. Example: `Debugger debugger(log);`. The stream is non-owning and must outlive use.

### `void set_ostream(std::ostream& ostream)`

Changes the output stream. Example: `debugger.set_ostream(log);`. Later event output uses the new stream; callback-managed streams must remain alive.

### `const std::ostream* get_ostream() const`

Returns the current non-owning stream pointer. Example: `auto* log = debugger.get_ostream();`. Read-only.

## Event tracking

### `void emit(event event)`

Stores the event, performs breakpoint checking for changed Runtime line/file events, and invokes the emit callback using the current output stream. Example: use through a debugger event callback only when you intentionally want to add an event. Emitting during a callback can recurse synchronously; avoid recursive emission.

### `void set_emit_event_slot(EmitSlotType func)`

Replaces the synchronous event callback. The callback receives an event by value and the output stream reference. Example: `debugger.set_emit_event_slot([](event e, std::ostream& o) { o << e.get_str(); });`. Passing an empty function leaves emission attempting to invoke an empty callback; do not do so. Changing it from inside its own callback changes later dispatches.

### `std::string get_next_event_str()`

Returns the event at the current cursor, then advances; when exhausted returns `EVENTS END`. Example: `auto first = debugger.get_next_event_str();`. It changes only the read cursor, not recorded events.

### `std::string get_all_events_strs()`

Returns formatted text for all stored events without changing the next-event cursor. Example: `auto log = debugger.get_all_events_strs();`. Read-only with respect to event records.

## Breakpoint control

### `void add_break_points(const std::vector<size_t>& points_buffer, const fs::path& FilePath = "")`

Adds line breakpoints, avoiding duplicate line/file records. Example: `debugger.add_break_points({3, 8}, "script.vlt");`. It adds to private state; there is no public remove/list method. Adding while the runtime is executing may change which later line stops.

### `void check_break_point(size_t line, Scopes::Scope* scope, const fs::path& FilePath = "")`

Checks a line/file against stored breakpoints and invokes the stop callback for matches. Example: normally the runtime calls this through `emit()`; direct host calls can invoke callbacks synchronously. Temporary used breakpoints are erased after the check. Calling it during active callback execution may recurse or mutate the breakpoint list currently being traversed.

### `void set_stop_slot(StopSlotType slot)`

Replaces the synchronous stop callback, receiving `Debugger&` and the active scope. Example: `debugger.set_stop_slot([](Debugger&, Scope*) { /* return to continue */ });`. When this callback returns, the invoking execution proceeds. An empty callback will be invoked on a stop and is invalid; always provide a callable. Changing it during a callback affects later stops.

### `void continue_exec()`

Currently has an empty implementation. The default console handler continues by leaving its own input loop; a custom stop callback can simply return. Example: do not rely on calling this method to resume a custom callback. It changes no state in this implementation.

### `void next_line()`

Adds a temporary breakpoint at `curent_line + 1` for the current file when one is not already found. Example: call from a stop callback to request a one-shot next-line stop. It changes private breakpoint state immediately; the next emitted runtime line determines whether it fires.

### `void exit()`

Prints a message and calls `std::exit(0)`, terminating the entire host process. Do not call it from an embedded host unless process termination is intended.

### `EventsT` values

`Info`, `Analysis`, `Runtime`, `Debug`, `Warning`, and `Error` are the full public enum values. They are classification constants; selecting one when constructing an event controls its label and does not itself cause a runtime action.

### `event::type`, `event::message`, `event::line`, `event::curent_scope`, and `event::file`

These fields hold the event category, message, source line, current non-owning scope pointer, and path. Example: `event.message`. Changing a callback's event copy changes only the copy; the `curent_scope` pointer still refers to runtime-owned memory and must not outlive its scope.

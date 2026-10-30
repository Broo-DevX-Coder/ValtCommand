/**
 * @file Runtime.hpp
 * @brief The ValtCommand runtime: runs scripts and exposes the embedding API.
 *
 * This header declares Runtime::RunTime, the class a C++ host uses to run
 * ValtCommand code from a string or a file, to register native C++ functions,
 * and to read or write script variables.
 *
 * @par Quick example
 * @code{.cpp}
 * Runtime::RunTime rt;
 *
 * auto r = rt.run("<script code>");
 * if (!r) {
 *     std::cerr << r.error.message
 *               << " (line " << r.error.line << ")\n";
 * }
 *
 * rt.run("<more code>");                              // state is kept between calls
 * rt.run("<temporary>", Runtime::ScopeMode::Fresh);   // state is not touched
 * rt.reset();                                         // clear all script state
 * @endcode
 */

#pragma once

// ==================================================================
// Macros
// ==================================================================

/**
 * @def API
 * @brief Export/import macro for the shared library.
 *
 * Expands to @c dllexport / @c visibility("default") while the library itself
 * is being built (@c BUILDING_COMPILER_DLL is defined), and to @c dllimport
 * (or nothing on non-Windows platforms) for the code that uses the library.
 */
#ifdef BUILDING_COMPILER_DLL
    #if defined(_WIN32)
        #define API __declspec(dllexport)
    #else
        #define API __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define API __declspec(dllimport)
    #else
        #define API 
    #endif
#endif

// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "API/globals.hpp"
#include "API/DevTools/Debugger.hpp"

// ==================================================================
// Forward Declarations
// ==================================================================
class ModuleNode;

// ==================================================================
// Runtime namespace
// ==================================================================

/**
 * @namespace Runtime
 * @brief Everything needed to run ValtCommand code from a C++ host.
 */
namespace Runtime {

    // ----------------------------------------------------------------
    // Result types (returned to the host instead of crashing it)
    // ----------------------------------------------------------------

    /**
     * @brief The struct of agrument that includes every thing about them
     */
    struct ExternalFunctionArgument {
        std::string type; ///< Type of data in var
        bool is_const; ///< Is the var const `can't change value`
        bool is_required; ///< Is the argument required
        bool is_any;  ///< Is the type of argument any (means the argument can be any type)
        Value default_value; ///< The default alue of argument in set function (arg<str> = "hh")
    };

    /**
     * @brief The stage of the pipeline that produced an error.
     */
    enum class ErrorStage {
        None,      ///< No error.
        IO,        ///< RunTime::run_file(): the file is missing or unreadable.
        Parse,     ///< Lexer or parser failed.
        Semantic,  ///< Semantic analysis failed.
        Runtime,   ///< Execution failed.
        Host       ///< Host API misuse (unknown variable, invalid type, ...).
    };

    /**
     * @brief Details of a failed operation.
     */
    struct RunError {
        ErrorStage stage = ErrorStage::None; ///< Where the error happened.
        std::string type;                    ///< Error class of the language ("NameError", "TypeError", ...), "" if none.
        std::string message;                 ///< Human-readable error message.
        int line = 0;                        ///< Line, numbered like Errors::ErrorInfo (0 = unknown).
        int column = 0;                      ///< Column, numbered like Errors::ErrorInfo (0 = unknown).
        Errors::ErrorInfo info;              ///< All info used in formatter
    };

    /**
     * @brief Result of RunTime::run(), RunTime::run_file(),
     *        RunTime::set_variable() and RunTime::get_variable().
     *
     * Failures are reported here instead of being thrown, so a bad script can
     * never crash the host. The result converts to @c bool, so
     * @code{.cpp}
     * if (!result) { ... }
     * @endcode
     * works as a failure check.
     */
    struct RunResult {
        bool ok = true;     ///< @c true if the operation succeeded.
        std::string value;  ///< Textual form of the last value ("" if none).
        std::string type;   ///< Type name of that value ("" if none).
        RunError error;     ///< Valid only when @ref ok is @c false.

        /// @return The same as @ref ok.
        explicit operator bool() const { return ok; }
    };

    /**
     * @brief How a call to RunTime::run() treats the global scope.
     */
    enum class ScopeMode {
        Persistent,  ///< Reuse the global scope: variables and functions survive between calls.
        Fresh        ///< Run in a temporary scope; the persistent state is untouched.
    };

    // ----------------------------------------------------------------
    // The program runtime
    // ----------------------------------------------------------------

    /**
     * @brief The program runtime.
     *
     * Owns the global scopes, the native (C++) functions and the optional
     * debugger, and runs ValtCommand code on top of them.
     *
     * @note The class is not copyable.
     * @see RunResult, ScopeMode
     */
    class API RunTime {
        private:

            /**
             * @brief A C++ function registered by the host.
             *
             * It is stored so that it can be registered again in new scopes
             * (after reset() or in ScopeMode::Fresh).
             */
            struct NativeFunction {
                std::string name;          ///< Name of function (needed to re-register it after reset()).
                ExternalFuncType function; ///< The function.
                std::string return_type;   ///< Return type of function.
                std::unordered_map<std::string, ExternalFunctionArgument> arguments; ///< All arguments of function.
                bool is_any = false;       ///< Does this function support any arguments?
            };

            std::vector<NativeFunction> native_functions; ///< All native functions.

            /**
             * @brief A constant registered by the host (like PI).
             *
             * It is stored so that it can be registered again in new scopes
             * (after reset() or in ScopeMode::Fresh).
             */
            struct NativeConstant {
                std::string name;   ///< Name of the constant.
                std::string type;   ///< Its type name ("int", "float", "str" or "bool").
                Value value;        ///< Its value.
            };

            std::vector<NativeConstant> native_constants; ///< All native constants.

            // Scopes ======
            std::unique_ptr<Scopes::Scope> run_scope;           ///< Global scope (main global scope and other modules' scopes) while running.
            std::unique_ptr<Scopes::Scope> run_semantic_scope;  ///< Global scope used for the analysis that comes right before a run.
            std::unique_ptr<Scopes::Scope> semantic_scope;      ///< Global scope used for pure analysis (semantic_analyses()).

            // Tools
            std::string Code;                                   ///< The last programming code.
            std::string FilePath;                               ///< Path of the current source file.
            std::unique_ptr<DevTools::Debugger> Debugger = nullptr; ///< The debugger of the runtime.

            // Info about the last run (filled by analyze / execute_code)
            ErrorStage last_stage = ErrorStage::None;           ///< Stage that failed (Parse / Semantic / Runtime).
            std::string last_value;                             ///< Text of the last value.
            std::string last_type;                              ///< Type name of the last value.

            // Internal helpers ======

            /// @brief (Re)creates the three global scopes and registers the native functions in them.
            void create_scopes();

            /// @brief Adds one native function to the three scopes.
            /// @param nf The function to add.
            void install_native(const NativeFunction& nf);

            /// @brief Adds one native constant to the three scopes.
            /// @param nc The constant to add.
            void install_constant(const NativeConstant& nc);

            /// @brief Registers every stored native function and constant into the current scopes.
            void register_natives();

            /**
             * @brief Analyzes, checks and executes @p code. Never throws.
             * @param code The code to run.
             * @param mode Which global scope to use.
             * @return The value of the run, or the error with its stage and position.
             */
            RunResult execute_source(const std::string& code, ScopeMode mode, const std::string& FilePath);

            /**
             * @brief Builds a failed RunResult.
             * @param stage   The stage that failed.
             * @param message The error message.
             * @param line    1-based line, 0 = unknown.
             * @param column  1-based column, 0 = unknown.
             * @return A RunResult with ok == false.
             */
            static RunResult make_error(ErrorStage stage, const std::string& message, int line = 0, int column = 0);

        public:

            // Lifetime ======

            /**
             * @brief Creates an empty runtime. Use run() or run_file() to give it code.
             * @param DebugMode The debug mode, is the debug allowed or no, default is `flase`
             */
            RunTime(bool DebugMode = false);

            /**
             * @brief Creates a runtime that already holds some code.
             * @param code The programming code (kept for compatibility with the old usage).
             * @param DebugMode The debug mode, is the debug allowed or no, default is `flase`
             */
            explicit RunTime(const std::string& code, bool DebugMode = false);

            /**
             * @brief Initialyze all native methods, debuger and all what the Runtime needs
             * @param DebugMode The debug mode, is the debug allowed or no, default is `flase`
             */
            void Initialyze(bool DebugMode = false);

            /**
             * @brief Gets the debugger owned by this runtime.
             * @return A pointer to the debugger, or @c nullptr when debugging is disabled.
             */
            DevTools::Debugger* GetDebugger();

            /**
             * @brief Sends an event to the runtime tracker.
             * @param event The event to record and dispatch.
             * @note Does nothing when debugging is disabled.
             */
            void emit_event(DevTools::event event);

            /**
             * @brief Sets the callback invoked for each runtime tracker event.
             * @param slot The host callback receiving each event.
             * @note Does nothing when debugging is disabled.
             */
            void set_emit_event_slot(DevTools::Debugger::EmitSlotType slot);

            /**
             * @brief Gets the next recorded runtime event as formatted text.
             * @return The next event string, "EVENTS END" when exhausted, or an empty string when debugging is disabled.
             */
            std::string get_next_event_str();

            /**
             * @brief Gets all recorded runtime events as formatted text.
             * @return The event strings, or an empty string when debugging is disabled.
             */
            std::string get_all_events_strs();

            /**
             * @brief Sets the stream used by the debugger for output.
             * @param output The output stream. It must outlive the debugger's use of it.
             * @note Does nothing when debugging is disabled.
             */
            void set_ostream(std::ostream& output);

            /**
             * @brief Gets the debugger output stream.
             * @return A pointer to the stream, or @c nullptr when debugging is disabled.
             */
            const std::ostream* get_ostream() const;

            /**
             * @brief Adds breakpoints at the requested source lines.
             * @param points The 1-based source line numbers.
             * @param FilePath The source file to match. An empty path matches any file.
             * @note Does nothing when debugging is disabled.
             */
            void add_break_points(
                const std::vector<size_t>& points,
                const std::string& FilePath = ""
            );

            /**
             * @brief Checks whether a source location matches a breakpoint.
             * @param line The 1-based source line number.
             * @param scope The scope active at that location, passed to the stop callback.
             * @param FilePath The source file path used for breakpoint matching.
             * @note Does nothing when debugging is disabled.
             */
            void check_break_point(
                size_t line,
                Scopes::Scope* scope,
                const std::string& FilePath = ""
            );

            /**
             * @brief Sets the callback invoked when execution pauses.
             * @param slot The host callback receiving the debugger and active scope.
             * @note Does nothing when debugging is disabled.
             */
            void set_stop_slot(DevTools::Debugger::StopSlotType slot);

            /**
             * @brief Resumes execution after a debugger pause.
             * @note Does nothing when debugging is disabled.
             */
            void continue_exec();

            /**
             * @brief Pauses execution at the next source line.
             * @note Does nothing when debugging is disabled.
             */
            void next_line();

            /**
             * @brief Stops execution through the debugger.
             * @note Does nothing when debugging is disabled.
             */
            void exit_debugging();

            RunTime(const RunTime&) = delete;            ///< Not copyable.
            RunTime& operator=(const RunTime&) = delete; ///< Not copyable.
            ~RunTime();

            // Embedding API ======

            /**
             * @brief Executes code from a string.
             *
             * Never throws: every failure is returned in the RunResult.
             *
             * @param code The code to execute.
             * @param mode ScopeMode::Persistent (default) keeps variables and functions
             *             between calls; ScopeMode::Fresh runs in a temporary scope.
             * @return The value of the run, or the error (stage, message, line, column).
             */
            RunResult run(const std::string& code, ScopeMode mode = ScopeMode::Persistent);

            /**
             * @brief Executes a script file.
             *
             * Same as run(), plus ErrorStage::IO if the file can't be read.
             *
             * @param path Path of the script file.
             * @param mode Which global scope to use.
             * @return The value of the run, or the error.
             */
            RunResult run_file(const std::string& path, ScopeMode mode = ScopeMode::Persistent);

            /**
             * @brief Clears all script state (variables, user functions, constants).
             * @note Native functions registered by the host are kept.
             */
            void reset();

            // Host <-> script variables (global scope) ======

            /**
             * @brief Creates or overwrites a global variable from the host.
             * @param name        Variable name.
             * @param value       The value, written as text (for example "42", "3.5", "true", "hello").
             * @param type        The type name: "int", "float", "bool" or "str".
             * @param is_constant Create it as a constant.
             * @return ok == true on success, or an ErrorStage::Host error
             *         (unknown type, or the text doesn't match the type).
             * @note If the variable already exists it is replaced.
             */
            RunResult set_variable(const std::string& name, const std::string& value, const std::string& type, bool is_constant = false);

            /**
             * @brief Reads a global variable.
             * @param name Variable name.
             * @return The value (as text) and type of the variable, or an error if it doesn't exist.
             */
            RunResult get_variable(const std::string& name);

            /**
             * @brief Checks whether a global variable exists.
             * @param name Variable name.
             * @return @c true if the variable exists.
             */
            bool has_variable(const std::string& name);

            // Native functions ======

            /**
             * @brief Adds an external C++ function that scripts can call.
             * @param function    The C++ function.
             * @param name        The name used in scripts.
             * @param return_type Return type of the function.
             * @param arguments   All arguments of the function (name -> argument).
             * @param is_any      @c true if the function supports any arguments.
             * @note The function is kept, so it survives reset() and is also
             *       available in ScopeMode::Fresh.
             */
            void add_external_function(ExternalFuncType function, const std::string& name, const std::string& return_type, std::unordered_map<std::string, ExternalFunctionArgument> arguments, bool is_any=false);

            /**
             * @brief Adds a native constant that scripts can read but not change (like PI).
             *
             * The type is taken from the value: @c int64_t is "int", @c long double is "float",
             * @c std::string is "str" and @c bool is "bool". Pass exactly one of these types
             * (for example @c 3.14L or @c int64_t{42}) to avoid an ambiguous conversion.
             *
             * @param name  The name used in scripts (for example "PI").
             * @param value The value of the constant.
             * @return @c false if the name is empty or the value is void, otherwise @c true.
             * @note Like native functions, the constant is kept: it survives reset()
             *       and exists in ScopeMode::Fresh. Adding the same name again replaces it.
             */
            bool add_external_constant(const std::string& name, const Value& value);

            // Low-level pipeline (used by run(), the debugger and the tests) ======

            /**
             * @brief Analyzes the code and builds the module node.
             * @param scope The scope used for the analysis.
             * @return The module node, or the error. On failure the stage
             *         (Parse or Semantic) is stored in the runtime.
             */
            ReturnResult<std::unique_ptr<ModuleNode>> analyze(Scopes::Scope* scope, const std::string& FilePath);

            /**
             * @brief Analyzes the code before running it, using the pure analysis scope.
             * @return @c true on success, or the error.
             */
            ReturnResult<bool> semantic_analyses();

            /**
             * @brief Analyzes the code and then executes it.
             * @return @c true on success, or the error.
             */
            ReturnResult<bool> execute_code();
    };
}

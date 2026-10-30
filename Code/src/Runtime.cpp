// ==================================================================
// Include necessary headers
// ==================================================================

// == Standard ==
#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

// == Locals ==
#include "API/Runtime.hpp"
#include "Errors.hpp"
#include "Lexar.hpp"
#include "utils.hpp"
#include "Parser.hpp"
#include "Scope.hpp"

// == Embeded Libs ==
#include "Libs/DevTools.hpp"
#include "Libs/Math.hpp"
#include "Libs/STDIO.hpp"
#include "Libs/Format.hpp"

// ==================================================================
// Private helpers (this file only)
// ==================================================================
namespace Runtime {
namespace {

    // Turn a failed ReturnResult into a RunResult (the host gets the type, message, line and column)
    RunResult failure_from(const ReturnResult<bool>& r, ErrorStage stage) {
        RunResult out;
        out.ok = false;
        out.error.stage = stage;
        out.error.type = r.error_info.type;
        out.error.message = r.error_info.message.empty() ? r.Message : r.error_info.message;
        out.error.line = static_cast<int>(r.error_info.line);
        out.error.column = static_cast<int>(r.error_info.column);
        out.error.info = r.error_info;
        return out;
    }

    // Turn a Value (int / float / string / bool) into text
    std::string value_to_text(const Value& v) {
        if (auto p = std::get_if<int64_t>(&v))     return std::to_string(*p);
        if (auto p = std::get_if<long double>(&v)) return fmt::format("{}", *p);
        if (auto p = std::get_if<std::string>(&v)) return *p;
        if (auto p = std::get_if<bool>(&v))        return *p ? "true" : "false";
        return ""; // void
    }

    // Get the language's type name of a Value ("int", "float", "str", "bool"; "" for void)
    std::string type_name_of(const Value& v) {
        if (std::holds_alternative<int64_t>(v))     return "int";
        if (std::holds_alternative<long double>(v)) return "float";
        if (std::holds_alternative<std::string>(v)) return "str";
        if (std::holds_alternative<bool>(v))        return "bool";
        return "";
    }

    // Turn text into a Value of the given type ("int", "float", "bool", "str").
    // Returns false (and fills `error`) if the text doesn't match the type.
    bool parse_value(const std::string& text, const std::string& type, Value& out, std::string& error) {
        try {
            std::size_t used = 0;

            if (type == "int") {
                long long v = std::stoll(text, &used);
                if (used != text.size()) throw std::invalid_argument("extra characters");
                out = static_cast<int64_t>(v);
            }
            else if (type == "float") {
                long double v = std::stold(text, &used);
                if (used != text.size()) throw std::invalid_argument("extra characters");
                out = v;
            }
            else if (type == "bool") {
                if (text == "true")       out = true;
                else if (text == "false") out = false;
                else throw std::invalid_argument("not a bool");
            }
            else if (type == "str") {
                out = text;
            }
            else {
                throw std::invalid_argument("unsupported type");
            }
            return true;
        }
        catch (const std::exception&) {
            error = "Cannot convert '" + text + "' to type '" + type + "'";
            return false;
        }
    }

    // Get the value of the script from the result of exec().
    // Only a top-level RETURN gives a value, otherwise the value stays empty.
    // (Template because exec() returns ReturnResult<T> and only its common members are used.)
    template <typename ExecResult>
    void describe_value(const ExecResult& result, std::string& text, std::string& type) {
        text.clear();
        type.clear();

        if (result.state == ExecState::Return) {
            text = value_to_text(result.return_data);
            type = type_name_of(result.return_data);
        }
    }

    // Turn the Runtime::ExternalFunctionArgument to Scopes::SymbolTableTypes::Argument
    Scopes::SymbolTableTypes::Argument ExternalFunctionArgument_to_Argument (const Runtime::ExternalFunctionArgument& arg__) {
        return Scopes::SymbolTableTypes::Argument {
            arg__.type,
            arg__.is_const,
            arg__.is_required,
            arg__.is_any,
            arg__.default_value
        };
    }

    // Turn unordered map of Runtime::ExternalFunctionArgument to unordered map of Scopes::SymbolTableTypes::Argument
    std::unordered_map<std::string, Scopes::SymbolTableTypes::Argument> unordered_map__ExternalFunctionArgument_to_Argument (const std::unordered_map<std::string, Runtime::ExternalFunctionArgument>& args__) {
        std::unordered_map<std::string, Scopes::SymbolTableTypes::Argument> args_list;
        for (auto& [name,arg]: args__)
            args_list[name] = ExternalFunctionArgument_to_Argument(arg);
        return args_list;
    }

}
}

// ==================================================================
// Runtime functions
// ==================================================================

// Constructor
Runtime::RunTime::RunTime(
    bool DebugMode
) {
    Initialyze(DebugMode);
}

// Constructor with code
Runtime::RunTime::RunTime(
    const std::string& code,
    bool DebugMode
): Code(code) {
    Initialyze(DebugMode);
}

// Initialyze every important thing
void 
Runtime::RunTime::Initialyze(
    bool DebugMode
) {
    // Initialyze all scopes
    create_scopes();

    // Create debugger
    if (DebugMode)
        Debugger = std::make_unique<DevTools::Debugger>();

    // Initialyze all native methods
    for (auto& func: {
        Lib::DevFunctions::__init__,
        Lib::Math::__init__,
        Lib::STDIO::__init__,
        Lib::Format::__init__
    })
        func(*this);
}

// Get the debugger pointer
DevTools::Debugger* 
Runtime::RunTime::GetDebugger() {
    return Debugger? Debugger.get() : nullptr;
}

void Runtime::RunTime::emit_event(DevTools::event event) {
    if (Debugger) Debugger->emit(std::move(event));
}

void Runtime::RunTime::set_emit_event_slot(DevTools::Debugger::EmitSlotType slot) {
    if (Debugger) Debugger->set_emit_event_slot(std::move(slot));
}

std::string Runtime::RunTime::get_next_event_str() {
    return Debugger ? Debugger->get_next_event_str() : std::string{};
}

std::string Runtime::RunTime::get_all_events_strs() {
    return Debugger ? Debugger->get_all_events_strs() : std::string{};
}

void Runtime::RunTime::set_ostream(std::ostream& output) {
    if (Debugger) Debugger->set_ostream(output);
}

const std::ostream* Runtime::RunTime::get_ostream() const {
    return Debugger ? Debugger->get_ostream() : nullptr;
}

void Runtime::RunTime::add_break_points(
    const std::vector<size_t>& points,
    const std::string& FilePath
) {
    if (Debugger) Debugger->add_break_points(points, FilePath);
}

void Runtime::RunTime::check_break_point(
    size_t line,
    Scopes::Scope* scope,
    const std::string& FilePath
) {
    if (Debugger) Debugger->check_break_point(line, scope, FilePath);
}

void Runtime::RunTime::set_stop_slot(DevTools::Debugger::StopSlotType slot) {
    if (Debugger) Debugger->set_stop_slot(std::move(slot));
}

void Runtime::RunTime::continue_exec() {
    if (Debugger) Debugger->continue_exec();
}

void Runtime::RunTime::next_line() {
    if (Debugger) Debugger->next_line();
}

void Runtime::RunTime::exit_debugging() {
    if (Debugger) Debugger->exit();
}

// (Re)create the three global scopes and put the native functions back in them
void
Runtime::RunTime::create_scopes() {
    run_scope = std::make_unique<Scopes::Scope>();
    semantic_scope = std::make_unique<Scopes::Scope>();
    run_semantic_scope = std::make_unique<Scopes::Scope>();
    register_natives();
}

// Add one native function to the three scopes
void
Runtime::RunTime::install_native(const NativeFunction& nf) {
    for (Scopes::Scope* scope : { run_scope.get(), run_semantic_scope.get(), semantic_scope.get() }) {
        auto f = scope->add_function(nf.name, nf.return_type, unordered_map__ExternalFunctionArgument_to_Argument(nf.arguments), nf.is_any);
        f->external_func = nf.function;
    }
}

// Add one native constant to the three scopes (is_const = true)
void
Runtime::RunTime::install_constant(const NativeConstant& nc) {
    run_scope->add_var(nc.name, nc.type, nc.value, true);
    run_semantic_scope->add_var(nc.name, nc.type, nc.value, true);
    semantic_scope->add_var(nc.name, nc.type, nc.value, true);
}

// Add every native function and constant to the current scopes
void
Runtime::RunTime::register_natives() {
    for (const auto& nf : native_functions) install_native(nf);
    for (const auto& nc : native_constants) install_constant(nc);
}

Runtime::RunResult
Runtime::RunTime::make_error(ErrorStage stage, const std::string& message, int line, int column) {
    RunResult r;
    r.ok = false;
    r.error.stage = stage;
    r.error.message = message;
    r.error.line = line;
    r.error.column = column;
    return r;
}

ReturnResult<Parser::PNode> 
Runtime::RunTime::analyze(Scopes::Scope* Scope, const std::string& FilePath) {

    last_stage = ErrorStage::Parse;

    Lexar l(Code);
    auto tokens_list = l.get_all_tokens();

    Parser p(tokens_list, FilePath);
    auto pars_result = p.get_module_node(FilePath);
    if (!pars_result.success) return pars_result;

    last_stage = ErrorStage::Semantic;

    auto accept_result = pars_result.value->accept({Scope, Debugger.get(), FilePath});
    if (!accept_result.success){
        ReturnResult<Parser::PNode> re = {accept_result.Message,false,nullptr};
        re.error_info = accept_result.error_info;
        return re;
    }

    return {"",true,std::move(pars_result.value)}; 
}

// Get the result of befor runing analyses
ReturnResult<bool> 
Runtime::RunTime::semantic_analyses() {
    auto r = analyze(semantic_scope.get(), FilePath);
    if (!r.success) {
        ReturnResult<bool> re = {r.Message,false,false};
        re.error_info = r.error_info;
        return re;
    }
    return {r.Message,true,true};
}

// Run thz code
ReturnResult<bool> 
Runtime::RunTime::execute_code() {
    auto analyze_result = analyze(run_semantic_scope.get(), FilePath);
    if (!analyze_result.success) {
        ReturnResult<bool> re = {analyze_result.Message,false,false};
        re.error_info = analyze_result.error_info;
        return re;
    }

    // std::cout << analyze_result.value->get_str() << std::endl; 

    last_stage = ErrorStage::Runtime;

    auto exec_result = analyze_result.value->exec({run_scope.get(), Debugger.get(), FilePath});

    if (!exec_result.success){
        ReturnResult<bool> re = {exec_result.Message,false,false};
        re.error_info = exec_result.error_info;
        return re;
    }

    describe_value(exec_result, last_value, last_type);

    return {"",true,true};
}

// analyze -> semantic check -> execute, and never throw
Runtime::RunResult
Runtime::RunTime::execute_source(const std::string& code, ScopeMode mode, const std::string& FilePath) {

    // Fresh mode: put the persistent scopes aside and work in new ones
    std::unique_ptr<Scopes::Scope> saved_run, saved_run_semantic, saved_semantic;
    if (mode == ScopeMode::Fresh) {
        saved_run = std::move(run_scope);
        saved_run_semantic = std::move(run_semantic_scope);
        saved_semantic = std::move(semantic_scope);
    }

    RunResult result;

    try {
        if (mode == ScopeMode::Fresh) create_scopes();

        Code = code;
        this->FilePath = FilePath;
        last_stage = ErrorStage::None;
        last_value.clear();
        last_type.clear();

        auto r = execute_code();
        if (!r.success) {
            result = failure_from(r, last_stage == ErrorStage::None ? ErrorStage::Runtime : last_stage);
        } else {
            result.ok = true;
            result.value = last_value;
            result.type = last_type;
        }
    }
    catch (const std::exception& e) {
        result = make_error(ErrorStage::Runtime, std::string("Internal error: ") + e.what());
    }
    catch (...) {
        result = make_error(ErrorStage::Runtime, "Internal error: unknown exception");
    }

    // Fresh mode: bring the persistent scopes back
    if (mode == ScopeMode::Fresh) {
        run_scope = std::move(saved_run);
        run_semantic_scope = std::move(saved_run_semantic);
        semantic_scope = std::move(saved_semantic);
    }

    return result;
}

// Run code from a string
Runtime::RunResult
Runtime::RunTime::run(const std::string& code, ScopeMode mode) {
    return execute_source(code, mode, "");
}

// Run code from a file
Runtime::RunResult
Runtime::RunTime::run_file(const std::string& path, ScopeMode mode) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return make_error(ErrorStage::IO, "Cannot open file: " + path);

    std::ostringstream buffer;
    buffer << in.rdbuf();
    std::string code = buffer.str();

    // Skip the UTF-8 BOM if the file has one
    if (code.size() >= 3 && code.compare(0, 3, "\xEF\xBB\xBF") == 0) code.erase(0, 3);

    return execute_source(code, mode, path);
}

// Clear all script state, keep the native functions
void
Runtime::RunTime::reset() {
    Code.clear();
    last_stage = ErrorStage::None;
    last_value.clear();
    last_type.clear();
    create_scopes();
}

// ==================================================================
// Host <-> script variables
// ==================================================================

// Create or overwrite a global variable
Runtime::RunResult
Runtime::RunTime::set_variable(const std::string& name, const std::string& value, const std::string& type, bool is_constant) {
    if (name.empty())
        return make_error(ErrorStage::Host, "The variable name is empty");

    for (const auto& nc : native_constants)
        if (nc.name == name)
            return make_error(ErrorStage::Host, "'" + name + "' is a native constant and can't be changed");

    if (type == "void" || !is_token_type_(type))
        return make_error(ErrorStage::Host, "Unknown or unsupported type '" + type + "'");

    Value parsed;
    std::string error;
    if (!parse_value(value, type, parsed, error))
        return make_error(ErrorStage::Host, error);

    // Same variable in the three scopes, so the analysis also knows it
    run_scope->add_var(name, type, parsed, is_constant);
    run_semantic_scope->add_var(name, type, parsed, is_constant);
    semantic_scope->add_var(name, type, parsed, is_constant);

    RunResult r;
    r.value = value_to_text(parsed);
    r.type = type;
    return r;
}

// Read a global variable
Runtime::RunResult
Runtime::RunTime::get_variable(const std::string& name) {
    Token t{TokenType::IDENTIFIER, name, 0, 0, 0};
    auto found = run_scope->search_var(t);
    if (!found.success)
        return make_error(ErrorStage::Host, "Unknown variable '" + name + "'");

    RunResult r;
    r.value = value_to_text(found.value->value);
    r.type = found.value->type;
    return r;
}

// Is there a global variable with this name
bool
Runtime::RunTime::has_variable(const std::string& name) {
    Token t{TokenType::IDENTIFIER, name, 0, 0, 0};
    return run_scope->search_var(t).success;
}

// ==================================================================
// Native functions
// ==================================================================
void
Runtime::RunTime::add_external_function(
    ExternalFuncType function, 
    const std::string& name, 
    const std::string& return_type,
    std::unordered_map<std::string, Runtime::ExternalFunctionArgument> arguments,
    bool is_any
) {
    NativeFunction nf;
    nf.name = name;
    nf.function = function;
    nf.return_type = return_type;
    nf.arguments = std::move(arguments);
    nf.is_any = is_any;

    native_functions.push_back(nf);  // kept, so reset() and Fresh mode can register it again
    install_native(nf);
}

// Add a constant that scripts can read but not change (like PI)
bool
Runtime::RunTime::add_external_constant(const std::string& name, const Value& value) {
    std::string type = type_name_of(value);
    if (name.empty() || type.empty()) return false;

    NativeConstant nc{name, type, value};

    // Same name again: replace the old one
    for (auto& old : native_constants) {
        if (old.name == name) {
            old = nc;
            install_constant(nc);
            return true;
        }
    }

    native_constants.push_back(nc);  // kept, so reset() and Fresh mode can register it again
    install_constant(nc);
    return true;
}

// Destructor of Runtime
Runtime::RunTime::~RunTime(){}

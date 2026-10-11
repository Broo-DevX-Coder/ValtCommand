// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Runtime.hpp" 
#include "Errors.hpp"
#include "Lexar.hpp"
#include "utils.hpp"

// ==================================================================
// Runtime functions
// ==================================================================

// Constructure
Runtime::RunTime::RunTime(
    const std::string& code
): Code(code) {
    run_scope = std::make_unique<Scopes::Scope>();
    semantic_scope = std::make_unique<Scopes::Scope>();
    run_semantic_scope = std::make_unique<Scopes::Scope>();
}

ReturnResult<Parser::PNode> 
Runtime::RunTime::analyze(Scopes::Scope* Scope) {

    Lexar l(Code);
    auto tokens_list = l.get_all_tokens();

    Parser p(tokens_list);
    auto pars_result = p.get_module_node();
    if (!pars_result.success) return pars_result;

    auto accept_result = pars_result.value->accept(Scope);
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
    auto r = analyze(semantic_scope.get());
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
    auto analyze_result = analyze(run_semantic_scope.get());
    if (!analyze_result.success) {
        ReturnResult<bool> re = {analyze_result.Message,false,false};
        re.error_info = analyze_result.error_info;
        return re;
    }

    auto exec_result = analyze_result.value->exec(run_scope.get());

    if (!exec_result.success){
        ReturnResult<bool> re = {exec_result.Message,false,false};
        re.error_info = exec_result.error_info;
        return re;
    }

    return {"",true,true};
}

void
Runtime::RunTime::add_external_function(
    ExternalFuncType function, 
    const std::string& name, 
    const std::string& return_type,
    std::unordered_map<std::string, Scopes::SymbolTableTypes::Method> methods,
    bool is_any
) {
    auto f = run_scope->add_function(name,return_type,methods,is_any);
    f->external_func = function;

    f = run_semantic_scope->add_function(name,return_type,methods,is_any);
    f->external_func = function;

    f = semantic_scope->add_function(name,return_type,methods,is_any);
    f->external_func = function;
}
// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Runtime.hpp" 
#include "Errors.hpp"
#include "Lexar.hpp"


// ==================================================================
// Scope functions
// ==================================================================

// Constructure
Scopes::Scope::Scope(
    Scope* parent_
): Parent(parent_) {};

// Add a function to functions list
Scopes::SymbolTableTypes::Function* 
Scopes::Scope::add_function(
    const std::string& name, 
    const std::string& return_type, 
    std::unordered_map<std::string, SymbolTableTypes::Method> methods
) {
    auto func_type = std::make_unique<SymbolTableTypes::Function>(
        SymbolTableTypes::Function{
            return_type,
            methods
        }
    );
    functions[name] = std::move(func_type);
    return functions[name].get();
}

// Add a variable to variables list
Scopes::SymbolTableTypes::RVar* 
Scopes::Scope::add_var(
    const std::string& name, 
    const std::string& type,
    Value& value,
    bool is_const
) {
    auto var_ptr = std::make_unique<SymbolTableTypes::RVar>(
        SymbolTableTypes::RVar{
            type,
            is_const,
            value
        }
    );
    variables[name] = std::move(var_ptr);
    return variables[name].get();
}

// Get a function by its name
ReturnResult<Scopes::SymbolTableTypes::Function*>
Scopes::Scope::search_function(
    Token& nToken
) {
    if (!functions.contains(nToken.value)){
        if (Parent != nullptr)  
            std::cout << nToken.value << " Serach on function" << std::endl << std::flush;
            return Parent->search_function(nToken);

        return {
            Errors::NameError(
                nToken.value,
                nToken.line,
                nToken.column,
                "Unknown function"
            ).msg,
            false,nullptr
        };
    }
    
    return {"",true,functions[nToken.value].get()};
}

// Get a function by its name
ReturnResult<Scopes::SymbolTableTypes::RVar*>
Scopes::Scope::search_var(
    Token& nToken
) {
    if (!variables.contains(nToken.value)) {
        if (Parent != nullptr)  
            return Parent->search_var(nToken);

        return {
            Errors::NameError(
                nToken.value,
                nToken.line,
                nToken.column,
                "Unknown variable"
            ).msg,
            false,nullptr
        };
    }

    return {"",true,variables[nToken.value].get()};
}

// ==================================================================
// Runtime functions
// ==================================================================

// Constructure
Runtime::RunTime::RunTime(
    const std::string& code
): Code(code) {
    

}

ReturnResult<Parser::PNode> 
Runtime::RunTime::analyze() {

    Lexar l(Code);
    auto tokens_list = l.get_all_tokens();

    Parser p(tokens_list);
    auto pars_result = p.get_module_node();

    if (!pars_result.success){
        return {pars_result.Message,false,nullptr};
    }

    auto accept_result = pars_result.value->accept(semantic_scope.get());

    if (!accept_result.success)
        return {accept_result.Message,false,nullptr};

    return {"",true,std::move(pars_result.value)}; 
}

// Get the result of befor runing analyses
ReturnResult<bool> 
Runtime::RunTime::semantic_analyses() {
    auto r = analyze();
    if (!r.success) 
        return {r.Message,false,false};
    return {r.Message,true,true};
}

// Run thz code
ReturnResult<bool> 
Runtime::RunTime::execute_code() {

    auto analyze_result = analyze();
    if (!analyze_result.success) 
        return {analyze_result.Message,false,false};

    auto exec_result = analyze_result.value->exec(run_scope.get());

    if (!exec_result.success)
        return {exec_result.Message,false,false};

    return {"",true,true};
}

void
Runtime::RunTime::add_external_function(
    ExternalFuncType function, 
    const std::string& name, 
    const std::string& return_type,
    std::unordered_map<std::string, Scopes::SymbolTableTypes::Method> methods
) {
    auto f = run_scope->add_function(name,return_type,methods);
    f->external_func = function;

    f = semantic_scope->add_function(name,return_type,methods);
    f->external_func = function;
}
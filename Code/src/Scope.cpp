// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Scope.hpp"
#include "utils.hpp"
#include "Errors.hpp"

// ==================================================================
// Scope functions
// ==================================================================

// Constructure
Scopes::Scope::Scope(
    Scope* parent_
): Parent(parent_) {
    ID = Utils::random_num();
    Depth = 0;
    if (parent_) Depth = parent_->get_depth() + 1;
};

// get ID of scope
size_t 
Scopes::Scope::get_id() {
    return ID;
}


// Get parent ID
Scopes::Scope* 
Scopes::Scope::get_parent() {
    return Parent;
}

// Get the depth of scope
size_t
Scopes::Scope::get_depth() {
    return Depth;
}

// Get a table of all visible functions ptrs in this scope
Scopes::Scope::funcs_table 
Scopes::Scope::get_functions_table() {
    // Initialyze result object
    funcs_table result;

    // Add parent's functions table if exists
    if (Parent) {
        auto parent_func_table = Parent->get_functions_table();
        result.insert(result.end(),parent_func_table.begin(),parent_func_table.end());
    }

    // Add this scope's functions
    for (auto& [name,obj]: functions) 
        result.push_back({name,obj.get()});

    return result;
}

// Get a table of all visible vars ptrs in this scope
Scopes::Scope::vars_table 
Scopes::Scope::get_variables_table() {
    // Initialyze result object
    vars_table result;

    // Add parent's variablles table if exists
    if (Parent) {
        auto parent_func_table = Parent->get_variables_table();
        result.insert(result.end(),parent_func_table.begin(),parent_func_table.end());
    }

    // Add this scope's variablles
    for (auto& [name,obj]: variables)
        result.push_back({name,obj.get()});

    return result;
}


// Add a function to functions list
Scopes::SymbolTableTypes::Function* 
Scopes::Scope::add_function(
    const std::string& name, 
    const std::string& return_type,
    std::unordered_map<std::string, SymbolTableTypes::Argument> args,
    bool is_any
) {
    auto func_type = std::make_unique<SymbolTableTypes::Function>(
        SymbolTableTypes::Function{
            return_type,
            args,
            is_any,
            ID
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
    const Value& value,
    bool is_const
) {
    auto var_ptr = std::make_unique<SymbolTableTypes::RVar>(
        SymbolTableTypes::RVar{
            type,
            is_const,
            value,
            ID
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
        if (Parent != nullptr)  {
            return Parent->search_function(nToken);
        }

        auto err = Errors::NameError(
            nToken.value,
            nToken.line,
            nToken.column,
            nToken.value.size(),
            "Unknown function"
        );

        ReturnResult<Scopes::SymbolTableTypes::Function*> re = {err.msg,false,nullptr}; 
        re.error_info = err.info;
        return re;
    }
    
    return {"",true,functions[nToken.value].get()};
}

// Get a function by its name
ReturnResult<Scopes::SymbolTableTypes::RVar*>
Scopes::Scope::search_var(
    Token& nToken
) {
    if (!variables.contains(nToken.value)) {
        if (Parent != nullptr){  
            return Parent->search_var(nToken);
        }

        auto err = Errors::NameError(
            nToken.value,
            nToken.line,
            nToken.column,
            nToken.value.size(),
            "Unknown variable"
        );

        ReturnResult<Scopes::SymbolTableTypes::RVar*> re = {err.msg,false,nullptr}; 
        re.error_info = err.info;
        return re;
    }

    return {"",true,variables[nToken.value].get()};
}

// Others

Scopes::SymbolTableTypes::Function::~Function() {
    delete user_function;
};
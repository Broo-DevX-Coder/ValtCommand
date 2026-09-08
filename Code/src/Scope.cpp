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
};

// get ID of scope
size_t 
Scopes::Scope::get_id() {
    return ID;
}

// Add a function to functions list
Scopes::SymbolTableTypes::Function* 
Scopes::Scope::add_function(
    const std::string& name, 
    const std::string& return_type,
    std::unordered_map<std::string, SymbolTableTypes::Method> methods,
    bool is_any
) {
    auto func_type = std::make_unique<SymbolTableTypes::Function>(
        SymbolTableTypes::Function{
            return_type,
            methods,
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
    Value& value,
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
        if (Parent != nullptr){  
            return Parent->search_var(nToken);
        }

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
#pragma once

// ==================================================================
// Marcos
// ==================================================================
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
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "globals.hpp"

// ==================================================================
// Scopes space
// ==================================================================
namespace Scopes {

    namespace SymbolTableTypes {
        enum class FunctionsTypes {
            Extenal, // An external function comes from pure c++
            Inside // The user function `SET_FUNCTION`
        };

        // Variable in scope
        struct SVar {
            std::string type; // Type of data in var
            bool is_const; // Is the var const `can't change value`
        };

        // Variable with value in scope
        struct RVar: public SVar {
            Value value; // The value of var
            size_t scope_id; // The scope id where the var created
        };

        // Function's method in scope
        struct Method: public SVar {
            bool is_required; // Is the method required 
            bool is_any;  // Is the type of method any (means the method can be any type)
            Value default_value; // The default alue of argument in set function (arg<str> = "hh")
        };
        
        // Function in scope
        struct Function {
            ~Function(); // Destructure
            std::string return_type; // The type of value returned
            std::unordered_map<std::string, Method> methods; // All methods of function
            bool is_sepport_any_methods_=false; // Is function can get any methods by any types (like print)
            size_t scope_id; // The id of scope in which this function was defined
            FunctionsTypes type; // The type of function (inside or external)
            ExternalFuncType external_func; // The pure c++ function if type is exernal
            ASTNode* user_function;  // A copy of Node of executing function if the type is inside
        };

    }

    // Scope
    class Scope {
        public: 
            struct VarsTableLine { std::string name; SymbolTableTypes::RVar* var_ptr; }; // A line of the variables table (used in inspectures)
            struct FuncsTableLine { std::string name; SymbolTableTypes::Function* func_ptr; }; // A line of the functions table (used in inspectures)
            using vars_table = std::vector<VarsTableLine>;
            using funcs_table = std::vector<FuncsTableLine>;

        protected:
            Scope* Parent; // The parent scope of this scope
            size_t ID; // The id of scope
            size_t Depth; // the depth of scope inside parents
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::Function>> functions; // All functions in scope
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::RVar>> variables; // All variables in scope

        public:
            Scope(Scope* parent = nullptr); // constructure
            size_t get_id(); // Get the ID of scope
            Scope* get_parent(); // Get parent ID
            size_t get_depth(); // Get the depth of scope
            
            funcs_table get_functions_table(); // Get a table of all visible functions ptrs in this scope
            vars_table get_variables_table(); // Get a table of all visible vars ptrs in this scope

            SymbolTableTypes::Function* add_function(const std::string& name, const std::string& return_type, std::unordered_map<std::string, SymbolTableTypes::Method> methods, bool is_any=false); // add function to scope table
            SymbolTableTypes::RVar* add_var(const std::string& name, const std::string& type, const Value& value, bool is_const=false); // add variable to scope table
            ReturnResult<SymbolTableTypes::Function*> search_function(Token& NameToken); // Get a function struct pointer from scope by name
            ReturnResult<SymbolTableTypes::RVar*> search_var(Token& NameToken); // Get a variable struct pointer from scope by name
    };
}
/**
 * @file scope.hpp
 * @brief scope declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "detail/globals.hpp"

/** @namespace Scopes @brief Runtime scopes and their symbol tables. */
namespace Scopes {

    namespace SymbolTableTypes {
        /** @brief Distinguishes host-provided and script-defined functions. */
        enum class FunctionsTypes {
            Extenal, ///< An external function comes from pure c++
            Inside ///< The user function `SET_FUNCTION`
        };

        /** @brief Variable metadata shared by declarations and runtime values. */
        struct SVar {
            std::string type; ///< Type of data in var
            bool is_const; ///< Is the var const `can't change value`
        };

        /** @brief A variable entry, including its current value and defining scope. */
        struct RVar: public SVar {
            Value value; ///< The value of var
            size_t scope_id; ///< The scope id where the var created
        };

        /** @brief Argument metadata stored for a function. */
        struct Argument: public SVar {
            bool is_required; ///< Is the argument required 
            bool is_any; ///< Is the type of argument any (means the argument can be any type)
            Value default_value; ///< The default alue of argument in set function (arg<str> = "hh")
        };
        
        /** @brief Function metadata and callback or script implementation. */
        struct Function {
            ~Function(); ///< Destructure
            std::string return_type; ///< The type of value returned
            std::unordered_map<std::string, Argument> arguments; ///< All arguments of function
            bool is_sepport_any_arguments_=false; ///< Is function can get any arguments by any types (like print)
            size_t scope_id; ///< The id of scope in which this function was defined
            FunctionsTypes type; ///< The type of function (inside or external)
            ExternalFuncType external_func; ///< The pure c++ function if type is exernal
            ASTNode* user_function; ///< A copy of Node of executing function if the type is inside
        };

    }

    /**
     * @brief Symbol table for one lexical scope.
     *
     * A scope can have a parent. Lookups and visible-table queries include
     * entries reachable through that parent chain.
     */
    class Scope {
        public: 
            /** @brief Name and entry pointer for a visible variable. */
            struct VarsTableLine { std::string name; SymbolTableTypes::RVar* var_ptr; };
            /** @brief Name and entry pointer for a visible function. */
            struct FuncsTableLine { std::string name; SymbolTableTypes::Function* func_ptr; };
            using vars_table = std::vector<VarsTableLine>;
            using funcs_table = std::vector<FuncsTableLine>;

        protected:
            Scope* Parent; ///< The parent scope of this scope
            size_t ID; ///< The id of scope
            size_t Depth; ///< the depth of scope inside parents
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::Function>> functions; ///< All functions in scope
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::RVar>> variables; ///< All variables in scope

        public:
            /** @brief Creates a scope, optionally nested under @p parent. */
            Scope(Scope* parent = nullptr);
            /** @brief Returns this scope's identifier. */
            size_t get_id();
            /** @brief Returns the parent scope, or null for a root scope. */
            Scope* get_parent();
            /** @brief Returns this scope's nesting depth. */
            size_t get_depth();
            
            /** @brief Returns pointers to all functions visible from this scope. */
            funcs_table get_functions_table();
            /** @brief Returns pointers to all variables visible from this scope. */
            vars_table get_variables_table();

            /** @brief Adds or replaces a function entry in this scope. */
            SymbolTableTypes::Function* add_function(const std::string& name, const std::string& return_type, std::unordered_map<std::string, SymbolTableTypes::Argument> arguments, bool is_any=false);
            /** @brief Adds or replaces a variable entry in this scope. */
            SymbolTableTypes::RVar* add_var(const std::string& name, const std::string& type, const Value& value, bool is_const=false);
            /** @brief Looks up a function using a token's name and source location. */
            ReturnResult<SymbolTableTypes::Function*> search_function(Token& NameToken);
            /** @brief Looks up a variable using a token's name and source location. */
            ReturnResult<SymbolTableTypes::RVar*> search_var(Token& NameToken);
    };
}

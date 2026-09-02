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
#include "Parser.hpp"

// ==================================================================
// Runtime namespace
// ==================================================================
namespace Runtime {

    // The program runtime
    class API RunTime {
        private:
            std::unique_ptr<Scopes::Scope> run_scope = std::make_unique<Scopes::Scope>(); // All global scopes (main global scope and other modules's scopes) in running
            std::unique_ptr<Scopes::Scope> semantic_scope = std::make_unique<Scopes::Scope>(); // All global scopes in analyzing
            std::string Code;
            
        public:
            RunTime(const std::string& code); // Constructure
            RunTime(RunTime& runtime) = delete; // 2nd constructure
            ReturnResult<Parser::PNode> analyze(); // Analyze the code and get the module node
            ReturnResult<bool> semantic_analyses(); // Analyse th code befor running
            ReturnResult<bool> execute_code(); // Start executing the code
            void add_external_function(ExternalFuncType function, const std::string& name, const std::string& return_type, std::unordered_map<std::string, Scopes::SymbolTableTypes::Method> methods); // Add extenal function of c++
    };
}
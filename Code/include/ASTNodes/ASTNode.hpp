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

// == Libs ==
#include <string>
#include <vector>
#include <memory>
#include <fmt/format.h>

// == Locals ==
#include "globals.hpp"

// ==================================================================
// ASTNodes Types
// ==================================================================
enum ASTNodesTypes {
    NT__ModuleNode,
    NT__FunctionCallNode,
    NT__FunctionCallArgumentNode,
    NT__ValueNode,
    NT__StringValueNode,
    NT__NumberValueNode,
    NT__BooleanValueNode
};

// ==================================================================
// ASTNode
// ==================================================================
class ASTNode
{
public:
    std::string return_type; // The type of return value
    virtual ~ASTNode() = default;
    virtual std::string get_str(int level) = 0; // Get the string of node to print AST
    virtual ASTNodesTypes NType() = 0; // get the type of node
    virtual ReturnResult<bool> accept(Scopes::Scope* ParentScope) = 0; // The node verifi it self befor runnig
    virtual ReturnResult<Value> exec(Scopes::Scope* ParentScope) = 0; // Run the node
};

// ==================================================================
// Main program node
// ==================================================================
class ModuleNode : public ASTNode
{
    public:
        using StatmentsT = std::vector<std::unique_ptr<ASTNode>>;
        
        StatmentsT statements;

        ModuleNode(StatmentsT& s_); // Constructure
        std::string get_str(int level=0) override; // get the str of node to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute node
};

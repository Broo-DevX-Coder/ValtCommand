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
#include "Scope.hpp"
#include "DevTools/Debugger.hpp"

// ==================================================================
// ASTNodes Types
// ==================================================================
enum ASTNodesTypes {
    NT__ModuleNode,
    NT__FunctionCallNode,
    NT__FunctionCallArgumentNode,
    NT__SetVariablNode,
    NT__ResetVariablNode,
    NT__GetVariableNode,
    NT__ValueNode,
    NT__StringValueNode,
    NT__NumberValueNode,
    NT__BooleanValueNode,
    NT__UserFunctionMethodNode,
    NT__UserFunctionNode,
    NT__UserFunctionReturnNode,
    NT__UserProxyFunctionNode,
    NT__BinOpsNode,
    NT__CompOpsNode,
    NT__LogicOpsNode,
    NT__SignOpNode,
    NT__PowerNode,
    NT__LogicalNotNode,
    NT__IfStatmentsNode,
    NT__WhileLoopsNode,
    NT__BreakLoopNode,
    NT__ContinueLoopNode,
    NT__ForLoopsNode
};

// ==================================================================
// Nodes accept and exec context
// ==================================================================
struct NodesContext {
    Scopes::Scope* ParentScope;
    DevTools::Debugger* Debugger;
};

// ==================================================================
// ASTNode
// ==================================================================
class ASTNode {
    public:
        std::string return_type; // The type of return value
        virtual ~ASTNode() = default;
        virtual std::string get_str(int level) = 0; // Get the string of node to print AST
        virtual ASTNodesTypes NType() = 0; // get the type of node
        virtual ReturnResult<bool> accept(NodesContext Context) = 0; // The node verifi it self befor runnig
        virtual ReturnResult<Value> exec(NodesContext Context) = 0; // Run the node
        virtual ASTNode* clone() = 0; // Clone the class or get a new copy from them
};

// ==================================================================
// Main program node
// ==================================================================
class ModuleNode : public ASTNode
{
    public:
        struct statment {
            std::unique_ptr<ASTNode> node;
            Token first_token;
        };
        using StatmentsT = std::vector<statment>;

    private:
        StatmentsT statements;
        Token FToken;

    public:
        ModuleNode(StatmentsT& s_,Token& First_token); // Constructure
        std::string get_str(int level=0) override; // get the str of node to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

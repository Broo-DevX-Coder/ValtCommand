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
#include <nlohmann/json.hpp>

// == Locals ==
#include "API/globals.hpp"
#include "Scope.hpp"
#include "API/DevTools/Debugger.hpp"

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
    NT__UserFunctionArgumentNode,
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
    std::string FilePath;
};

// ==================================================================
// ASTNode
// ==================================================================
class ASTNode {
    public:
        std::string return_type; // The type of return value
        std::string file; // The path of file that encludes the nodes
        Token END_token; // Token marking the end location associated with this node
        ASTNode(const std::string& FilePath, const Token& END_token); // Constructure
        void emit_exec_end_event(const NodesContext& Context) const;
        virtual ~ASTNode()= default; 
        virtual std::string get_str(int level) = 0; // Get the string of node to print AST
        virtual nlohmann::json get_json() = 0; // Get the json file of ASTNode
        virtual ASTNodesTypes NType() = 0; // get the type of node
        virtual ReturnResult<bool> accept(NodesContext Context) = 0; // The node verifi it self befor runnig
        virtual ReturnResult<Value> exec(NodesContext Context) = 0; // Run the node
        virtual ASTNode* clone() = 0; // Clone the class or get a new copy from them
};

class ASTNodeExecCompletionGuard {
private:
    const ASTNode& node;
    const NodesContext& context;

public:
    ASTNodeExecCompletionGuard(const ASTNode& node, const NodesContext& context)
        : node(node), context(context) {}
    ~ASTNodeExecCompletionGuard() { node.emit_exec_end_event(context); }
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
        ModuleNode(const std::string& FilePath, const Token& END_token, StatmentsT& s_,Token& First_token); // Constructure
        std::string get_str(int level=0) override; // get the str of node to print
        nlohmann::json get_json(); // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

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
#include "ASTNodes/ASTNode.hpp"

// ==================================================================
// Classes of break and continue node
// ==================================================================

// Break loop
class BreakLoopNode: public ASTNode {
    private:
        Token BreakT;
    public:
        BreakLoopNode(Token& token);
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute the loop
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// Continue loop
class ContinueLoopNode: public ASTNode {
    private:
        Token ContinueT;
    public:
        ContinueLoopNode(Token& token);
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute the loop
        ASTNode* clone() override; // Clone the class then get a new copy from them
};

// ==================================================================
// Class of while loops
// ==================================================================

// While loops node
class WhileLoopsNode: public ASTNode {
    public:
        using Node = std::unique_ptr<ASTNode>;
        using NodesL = std::vector<Node>;

    private:
        Node condition_node; // Condition of whiile loop
        NodesL Statments; // All statments that will execte and repete if condition is true
        Token FirstToken; // The first token of loop definition

    public:
        WhileLoopsNode(Token& FirstToken, Node& condition, NodesL& nodes_list); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute the loop
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// ==================================================================
// Class of for loops
// ==================================================================

// For loops node
class ForLoopsNode: public ASTNode {
    public:
        using Node = std::unique_ptr<ASTNode>;
        using NodesL = std::vector<Node>;
        struct Component {
            Token first_token;
            Node node;
        };

    private:
        Component Initialization; // Initialyze the variable of condition one time
        Component Condition; // The condition of continuation of loop 
        Component Update; // Update the condition's var
        NodesL Statments; // All instructions of loop excuting

    public:
        ForLoopsNode(Component& Initialization, Component& Condition, Component& Update, NodesL& statments); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute the loop
        ASTNode* clone() override; // Clone the class or get a new copy from them
};
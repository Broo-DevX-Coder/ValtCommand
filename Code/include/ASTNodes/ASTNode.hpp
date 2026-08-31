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

// ==================================================================
// ASTNodes Types
// ==================================================================
enum ASTNodesTypes {
    T__ProgramNode,
    T__FunctionCallNode,
    T__FunctionCallArgumentNode
};

// ==================================================================
// ASTNode
// ==================================================================
class API ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual std::string get_str(int level) = 0;
    virtual ASTNodesTypes NType() = 0;
    virtual void exec() = 0;
};

// ==================================================================
// Main program node
// ==================================================================
class ProgramNode : public ASTNode
{
    public:
        using StatmentsT = std::vector<std::unique_ptr<ASTNode>>;
        
        StatmentsT statements;

        ProgramNode(StatmentsT& s_); // Constructure
        std::string get_str(int level) override; // get the str of node to print
        ASTNodesTypes NType() override; // Get the type of node
        void exec() override; // Execute node
};


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
    virtual ~ASTNode() = default;
    virtual std::string get_str(int level) = 0;
    virtual ASTNodesTypes NType() = 0;
    virtual ReturnResult<Value> exec() = 0;
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
        ReturnResult<Value> exec() override; // Execute node
};


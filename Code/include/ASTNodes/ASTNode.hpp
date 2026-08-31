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

// ==================================================================
// ASTNode
// ==================================================================
class API ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual std::string get_str(int level) = 0;
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
};


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
#include <vector>
#include <unordered_map>

// == Locals ==
#include "ASTNodes/ASTNode.hpp"
#include "globals.hpp"

// ==================================================================
// Function Call Argument Node
// ==================================================================

// Function Call Argument Node
class API FunctionCallArgumentNode: public ASTNode
{
    public:
        std::string name; // Name of argument
        std::string type; // Type of argument
        std::string value_str; // The str of value
        Value value; // Value that in argumen

        FunctionCallArgumentNode(std::string n_, std::string t_, std::string v_); // Constructure
        std::string get_str(int level) override; // Get str of node to print
};


// ==================================================================
// Function Call Node
// ==================================================================

// Function Call Node
class API FunctionCallNode: public ASTNode
{
    using ArgsT = std::vector<FunctionCallArgumentNode>;

    public:
        std::string name; // Name of function
        ArgsT arguments; // Function arguments

        FunctionCallNode(std::string n_, ArgsT& a_); // Contructure
        std::string get_str(int level) override; // Get str of node to print
};


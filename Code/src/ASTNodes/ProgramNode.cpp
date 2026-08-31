// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <sstream>

// == Locals ==
#include "ASTNodes/ASTNode.hpp"

// ==================================================================
// Proogram node functions
// ==================================================================

// Constructure
ProgramNode::ProgramNode(
    StatmentsT& s_
): statements(std::move(s_)) {}

// get str to print
std::string
ProgramNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Program:" << "\n";

    for (auto& statement: statements)
        ss << statement->get_str(level+1);

    return ss.str();
}
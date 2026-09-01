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
ModuleNode::ModuleNode(
    StatmentsT& s_
): statements(std::move(s_)) {}

// get str to print
std::string
ModuleNode::get_str(
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

// Get the node type
ASTNodesTypes 
ModuleNode::NType() {
    return NT__ModuleNode;
}

// Execute node
ReturnResult<Value>
ModuleNode::exec() {
    ReturnResult<Value> r;
    for (auto& stmt: statements) {
        r = stmt->exec();
        if (!r.success)
            return {r.Message,false,std::monostate()};
    }
    return {"",true,std::monostate()};
}
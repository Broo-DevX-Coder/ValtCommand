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
): statements(std::move(s_)) {
    return_type = "void";
}

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
ModuleNode::exec(
    Scopes::Scope* ParentScope
) {
    ReturnResult<Value> r;
    for (auto& stmt: statements) {
        r = stmt->exec(ParentScope);
        if (!r.success)
            return {r.Message,false,std::monostate()};
        stmt = nullptr;
    }
    return {"",true,std::monostate()};
}

// Execute node
ReturnResult<bool>
ModuleNode::accept(
    Scopes::Scope* ParentScope
) {
    ReturnResult<bool> r;

    for (auto& stmt: statements) {
        r = stmt->accept(ParentScope);

        if (!r.success)
            return {r.Message,false,false};
    }
    return {"",true,true};
}

// Copy node
ASTNode* 
ModuleNode::clone() {
    StatmentsT new_statments;

    for (auto& s: statements) {
        std::unique_ptr<ASTNode> statment_c(s->clone());
        new_statments.push_back(std::move(statment_c));
    }

    return new ModuleNode(new_statments);
}
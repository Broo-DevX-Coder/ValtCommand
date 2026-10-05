// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/IfElseNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// If statment node
// ==================================================================

// Constructure
IfStatmentsNode::IfStatmentsNode(
    Token& CFtoken,
    Node& condition, 
    Nodes_list& statments
): 
    condition_FT(CFtoken),
    condition_node(std::move(condition)),
    Statments(std::move(statments)) {}


// Get the str to print
std::string 
IfStatmentsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "If statment \n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Condition: \n" << condition_node->get_str(level+2);

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Statments: \n";

    for (auto& stm: Statments) 
        ss << stm->get_str(level+2);

    return ss.str();
}

// Get the type of node
ASTNodesTypes 
IfStatmentsNode::NType() {
    return NT__IfStatmentsNode;
}

// Clone the class or get a new copy from them
ASTNode* 
IfStatmentsNode::clone() {
    Node new_condition(condition_node?condition_node->clone():nullptr);
    Nodes_list new_stms_list;

    for (auto& stm: Statments) {
        Node new_stm(stm?stm->clone():nullptr);
        new_stms_list.push_back(std::move(new_stm));
    }

    return new IfStatmentsNode(
        condition_FT,
        new_condition,
        new_stms_list
    );
}

// types and value checking
ReturnResult<bool> 
IfStatmentsNode::accept(
    Scopes::Scope* ParentScope
) {
    // Set return type to void
    return_type = "void";

    // Verifi condition node
    auto r = condition_node->accept(ParentScope);
    if (!r.success) return r;

    // Verifi condition type
    if (condition_node->return_type != "bool")
        return {Errors::TypeError(
            "bool",
            condition_node->return_type,
            condition_FT.line,
            condition_FT.column
        ).msg,false,false};

    // Verifi all Statments
    for (auto& smt: Statments) {
        auto r = smt->accept(ParentScope);
        if (!r.success) return r;
    }

    return {"",true,true};
}

// execute and get the result of calculation
ReturnResult<Value> 
IfStatmentsNode::exec(
    Scopes::Scope* ParentScope
) {
    // Execute condition and get its result
    auto condition_result = condition_node->exec(ParentScope);
    if (!condition_result.success) return condition_result;

    // Delete condition node
    condition_node = nullptr;

    // Execute statments if condition is true
    if (condition_result.value == Value(true))  {
        for (auto& stm: Statments) {
            auto r = stm->exec(ParentScope); // Exucute statment
            if (!r.success) return r; // return statment if there is an error
            if (r.state != ExecState::Normal)
                return r;
            stm = nullptr; // Delete statment
        }
    }

    return {"",true,std::monostate()};
}
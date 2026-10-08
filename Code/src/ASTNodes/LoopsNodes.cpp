// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/LoopsNodes.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Classes of break and continue node
// ==================================================================

// Get the str to print
std::string 
BreakLoopNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Break \n";

    return ss.str();
}

// Get the str to print
std::string 
ContinueLoopNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Continue \n";

    return ss.str();
}

// Get the type of node
ASTNodesTypes 
BreakLoopNode::NType() {
    return NT__BreakLoopNode; 
}

// Get the type of node
ASTNodesTypes 
ContinueLoopNode::NType() {
    return NT__ContinueLoopNode; 
}

// types and value checking
ReturnResult<bool> 
BreakLoopNode::accept(Scopes::Scope* ParentScope) {
    return {"",true,true,ExecState::Break};
}

// types and value checking
ReturnResult<bool> 
ContinueLoopNode::accept(Scopes::Scope* ParentScope) {
    return {"",true,true,ExecState::Continue};
}

// execute the loop
ReturnResult<Value> 
BreakLoopNode::exec(Scopes::Scope* ParentScope) {
    return {"",true,std::monostate{},ExecState::Break};
}

// execute the loop
ReturnResult<Value> 
ContinueLoopNode::exec(Scopes::Scope* ParentScope) {
    return {"",true,std::monostate{},ExecState::Continue};
}

// Clone the class then get a new copy from them
ASTNode* 
BreakLoopNode::clone() {
    return new BreakLoopNode();
}

// Clone the class then get a new copy from them
ASTNode* 
ContinueLoopNode::clone() {
    return new ContinueLoopNode();
}

// ==================================================================
// While loop node functions
// ==================================================================

// Constructure
WhileLoopsNode::WhileLoopsNode(
    Token& FirstToken, 
    Node& condition, 
    NodesL& nodes_list

):  condition_node(std::move(condition)),
    FirstToken(FirstToken),
    Statments(std::move(nodes_list)) {
    return_type = "void";
}

// Get the str to print
std::string 
WhileLoopsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "While Loop \n";

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
WhileLoopsNode::NType() {
    return NT__WhileLoopsNode; 
}

// Clone the class or get a new copy from them
ASTNode* 
WhileLoopsNode::clone() {
    Node new_condition(condition_node?condition_node->clone():nullptr);
    NodesL new_stms_list;

    for (auto& stm: Statments) {
        Node new_stm(stm?stm->clone():nullptr);
        new_stms_list.push_back(std::move(new_stm));
    }

    return new WhileLoopsNode(
        FirstToken,
        new_condition,
        new_stms_list
    );
}

// types and value checking
ReturnResult<bool> 
WhileLoopsNode::accept(
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
            FirstToken.line,
            FirstToken.column
        ).msg,false,false};

    // Create the returning nodes list
    std::vector<ASTNode*> return_nodes;

    // Create a specific scoupe for statments block
    auto while_scoupe = std::make_unique<Scopes::Scope>(ParentScope);

    // Verifi all Statments
    for (auto& smt: Statments) {
        auto r = smt->accept(while_scoupe.get());
        if (!r.success) return r;
        
        if (r.state == ExecState::Return) {
            for (auto rptr: r.return_nodes)
                return_nodes.push_back(rptr);
        }
    }

    return {"",true,true,return_nodes.empty()?ExecState::Normal:ExecState::Return,std::move(return_nodes)};
}

// execute the node
ReturnResult<Value> 
WhileLoopsNode::exec(
    Scopes::Scope* ParentScope
) {
    return_type = "void";

    ReturnResult<Value> condition_result;
    
    {
        // Copy condition node
        auto condition_node_cp_raw_ptr = condition_node->clone();
        Node condition_node_cp(condition_node_cp_raw_ptr);

        // Execute condition and get its result
        condition_result = condition_node_cp->exec(ParentScope);
        if (!condition_result.success) return condition_result;
    }

    // Execute statments if condition is true
    bool steel_running = true;
    while (condition_result.value == Value(true) && steel_running)  {

        // Create a specific scoupe for if statment
        auto while_scoupe = std::make_unique<Scopes::Scope>(ParentScope);

        for (auto& stm: Statments) {
            // Create a copy of node
            auto stm_raw_ptr = stm->clone();
            Node stm_ptr(stm_raw_ptr);

            // execute node
            auto r = stm_ptr->exec(while_scoupe.get()); // Exucute statment
            if (!r.success) return r; // return statment if there is an error

            if (r.state == ExecState::Return) return r; // If there is a return of data
            if (r.state == ExecState::Continue) break; // stop executing all statments and reenter in new loop
            if (r.state == ExecState::Break) steel_running = false; // stop all the node
        }
        
        if (steel_running){
            // Copy condition node
            auto condition_node_cp_raw_ptr = condition_node->clone();
            Node condition_node_cp(condition_node_cp_raw_ptr);

            // Execute condition and get its result
            condition_result = condition_node_cp->exec(ParentScope);
            if (!condition_result.success) return condition_result;
        }
    }

    // Delete all statments
    Statments.clear();

    // Delete condition node
    condition_node = nullptr;

    return {"",true,std::monostate()};
}
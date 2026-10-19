// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/VariableNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>
#include <string>
#include <utility>

// ==================================================================
// Runtime Tracker helper
// ==================================================================

static void emit_event(
    NodesContext& Context,
    DevTools::EventsT type,
    const std::string& message,
    std::size_t line
) {
    if (Context.Debugger) {
        Context.Debugger->emit({
            type,
            message,
            line
        });
    }
}

// ==================================================================
// Set variable node class functions
// ==================================================================

// Constructor
SetVariableNode::SetVariableNode(
    Token name_token,
    Token type_token,
    std::unique_ptr<ASTNode> value_node_,
    bool is_const_
):
    NameToken(name_token),
    TypeToken(type_token),
    name(name_token.value),
    type(type_token.value),
    is_const(is_const_),
    VNode(std::move(value_node_)) {}


// Get the string representation
std::string 
SetVariableNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    if (is_const)
        ss << "Const " << name << ":\n";
    else
        ss << "Variable " << name << ":\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "type: " << type << "\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "value: \n";

    ss << VNode->get_str(level + 2);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
SetVariableNode::get_json() {
    nlohmann::json result;

    result["type"] = is_const ? "ConstantDeclaration"
                              : "VariableDeclaration";

    result["name"] = name;
    result["value_type"] = type;
    result["value"] = VNode->get_json();

    return result;
}

// Get the node type
ASTNodesTypes
SetVariableNode::NType() {
    return NT__SetVariablNode;
}


// Type and value checking
ReturnResult<bool>
SetVariableNode::accept(
    NodesContext Context
) {
    return_type = "void";

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing variable declaration '" + name +
            "' with type '" + type + "'",
        NameToken.line
    );

    auto parent_scope_id = Context.ParentScope->get_id();
    auto search_r = Context.ParentScope->search_var(NameToken);

    Errors::VariableError err(
        name,
        NameToken.line,
        NameToken.column,
        NameToken.value.size()
    );

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Verifying if variable '" + name +
            "' already exists in the current scope",
        NameToken.line
    );

    if (search_r.success) {
        if (parent_scope_id == search_r.value->scope_id) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Variable declaration failed: '" + name +
                    "' already exists in the current scope",
                NameToken.line
            );

            ReturnResult<bool> re = {
                err.reset_var_in_same_scope(),
                false,
                false
            };

            re.error_info = err.info;
            return re;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing initial value of variable '" + name + "'",
        NameToken.line
    );

    auto vnode_ar = VNode->accept(Context);

    if (!vnode_ar.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Variable declaration failed while analyzing the initial value of '" +
                name + "'",
            NameToken.line
        );

        return vnode_ar;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Checking type compatibility: expected '" + type +
            "', got '" + VNode->return_type + "'",
        TypeToken.line
    );

    if (!are_types_compatible(VNode->return_type, type)) {
        auto type_err = Errors::TypeError(
            type,
            VNode->return_type,
            TypeToken.line,
            TypeToken.column,
            TypeToken.value.size()
        );

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Variable declaration failed: incompatible type for '" +
                name + "'",
            TypeToken.line
        );

        ReturnResult<bool> re = {
            type_err.msg,
            false,
            false
        };

        re.error_info = type_err.info;
        return re;
    }

    Value v = "";

    Context.ParentScope->add_var(
        name,
        type,
        v,
        is_const
    );

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Variable '" + name +
            "' passed analysis and was registered in the scope",
        NameToken.line
    );

    return {"", true, true};
}


// Execute and set the variable in the symbol table
ReturnResult<Value>
SetVariableNode::exec(
    NodesContext Context
) {
    return_type = "void";

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing variable declaration '" + name + "'",
        NameToken.line
    );

    auto vnode_ar = VNode->exec(Context);

    if (!vnode_ar.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Variable declaration failed while evaluating the initial value of '" +
                name + "'",
            NameToken.line
        );

        return vnode_ar;
    }

    auto value = reconsiliation_int_float(
        type,
        vnode_ar.value
    );

    Context.ParentScope->add_var(
        name,
        type,
        value,
        is_const
    );

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Variable '" + name +
            "' was initialized successfully",
        NameToken.line
    );

    return {"", true, std::monostate{}};
}


// Copy node
ASTNode*
SetVariableNode::clone() {
    std::unique_ptr<ASTNode> new_value_node(
        VNode->clone()
    );

    return new SetVariableNode(
        NameToken,
        TypeToken,
        std::move(new_value_node),
        is_const
    );
}


// ==================================================================
// Reset variable value node class functions
// ==================================================================

// Constructor
ResetVariableNode::ResetVariableNode(
    Token name_token,
    std::unique_ptr<ASTNode> value_node_
):
    NameToken(name_token),
    name(name_token.value),
    VNode(std::move(value_node_)) {}


// Get the string representation
std::string
ResetVariableNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Reset Variable " << name << ":\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "value: \n";

    ss << VNode->get_str(level + 2);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
ResetVariableNode::get_json() {
    nlohmann::json result;

    result["type"] = "VariableAssignment";
    result["name"] = name;
    result["value"] = VNode->get_json();

    return result;
}

// Get the node type
ASTNodesTypes
ResetVariableNode::NType() {
    return NT__ResetVariablNode;
}


// Type and value checking
ReturnResult<bool>
ResetVariableNode::accept(
    NodesContext Context
) {
    return_type = "void";

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing assignment to variable '" + name + "'",
        NameToken.line
    );

    auto parent_scope_id = Context.ParentScope->get_id();
    auto search_r = Context.ParentScope->search_var(NameToken);

    Errors::VariableError err(
        name,
        NameToken.line,
        NameToken.column,
        NameToken.value.size()
    );

    // Variable not found
    if (!search_r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Assignment failed: variable '" + name + "' was not found",
            NameToken.line
        );

        ReturnResult<bool> re = {
            search_r.Message,
            false,
            false
        };

        re.error_info = search_r.error_info;
        return re;
    }

    auto var_ptr = search_r.value;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Variable '" + name + "' was found; checking whether it is constant",
        NameToken.line
    );

    // Variable is const
    if (var_ptr->is_const) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Assignment failed: variable '" + name + "' is constant",
            NameToken.line
        );

        ReturnResult<bool> re = {
            err.reset_const_var(),
            false,
            false
        };

        re.error_info = err.info;
        return re;
    }

    // Variable is in another scope
    // if (parent_scope_id != search_r.value->scope_id) {
    //     return {err.reset_var_from_auther_scope(), false, false};
    // }

    type = var_ptr->type;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing the new value assigned to variable '" + name + "'",
        NameToken.line
    );

    auto vnode_ar = VNode->accept(Context);

    if (!vnode_ar.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Assignment failed while analyzing the new value of '" +
                name + "'",
            NameToken.line
        );

        return vnode_ar;
    }

    // Value type is incompatible with variable type
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Checking assignment type compatibility: expected '" + type +
            "', got '" + VNode->return_type + "'",
        NameToken.line
    );

    if (!are_types_compatible(VNode->return_type, type)) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Assignment failed: incompatible value type for variable '" +
                name + "'",
            NameToken.line
        );

        return {
            err.reset_var_value_by_other_type(
                type,
                VNode->return_type
            ),
            false,
            false
        };
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Assignment to variable '" + name +
            "' passed analysis",
        NameToken.line
    );

    return {"", true, true};
}


// Execute node
ReturnResult<Value>
ResetVariableNode::exec(
    NodesContext Context
) {
    return_type = "void";

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing assignment to variable '" + name + "'",
        NameToken.line
    );

    auto vnode_ar = VNode->exec(Context);

    if (!vnode_ar.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Assignment failed while evaluating the new value of '" +
                name + "'",
            NameToken.line
        );

        return vnode_ar;
    }

    auto value = vnode_ar.value;

    auto search_r = Context.ParentScope->search_var(NameToken);

    if (!search_r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Assignment failed: variable '" + name +
                "' could not be found during execution",
            NameToken.line
        );

        ReturnResult<Value> re = {
            search_r.Message,
            false,
            std::monostate{}
        };

        re.error_info = search_r.error_info;
        return re;
    }

    search_r.value->value = reconsiliation_int_float(
        type,
        value
    );

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Variable '" + name + "' was updated successfully",
        NameToken.line
    );

    return {"", true, std::monostate{}};
}


// Copy node
ASTNode*
ResetVariableNode::clone() {
    std::unique_ptr<ASTNode> new_value_node(
        VNode->clone()
    );

    return new ResetVariableNode(
        NameToken,
        std::move(new_value_node)
    );
}


// ==================================================================
// Get variable value node class functions
// ==================================================================

// Constructor
GetVariableNode::GetVariableNode(
    Token name_token
):
    NameToken(name_token),
    name(name_token.value) {}


// Get the string representation
std::string
GetVariableNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Get Variable " << name << "\n";

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
GetVariableNode::get_json() {
    nlohmann::json result;

    result["type"] = "VariableAccess";
    result["name"] = name;

    return result;
}

// Get the node type
ASTNodesTypes
GetVariableNode::NType() {
    return NT__GetVariableNode;
}


// Type and value checking
ReturnResult<bool>
GetVariableNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing access to variable '" + name + "'",
        NameToken.line
    );

    auto search_r = Context.ParentScope->search_var(NameToken);

    // Variable not found
    if (!search_r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Variable access failed: '" + name + "' was not found",
            NameToken.line
        );

        ReturnResult<bool> re = {
            search_r.Message,
            false,
            false
        };

        re.error_info = search_r.error_info;
        return re;
    }

    return_type = search_r.value->type;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Variable '" + name +
            "' was found; its type is '" + return_type + "'",
        NameToken.line
    );

    return {"", true, true};
}


// Execute node
ReturnResult<Value>
GetVariableNode::exec(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Reading the value of variable '" + name + "'",
        NameToken.line
    );

    auto search_r = Context.ParentScope->search_var(NameToken);

    if (!search_r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Variable read failed: '" + name +
                "' was not found during execution",
            NameToken.line
        );

        ReturnResult<Value> re = {
            search_r.Message,
            false,
            std::monostate{}
        };

        re.error_info = search_r.error_info;
        return re;
    }

    return_type = search_r.value->type;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Variable '" + name + "' was read successfully",
        NameToken.line
    );

    return {"", true, search_r.value->value};
}


// Copy node
ASTNode*
GetVariableNode::clone() {
    return new GetVariableNode(NameToken);
}
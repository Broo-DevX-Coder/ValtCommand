
 // ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/OneSideOpsNodes.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Helper: emit a Runtime Tracker event
// ==================================================================

static void emit_sign_event(
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
// Helper: convert sign operation to string
// ==================================================================

static std::string sign_to_string(SignOpsTypes type) {
    switch (type) {
        case SignOpsTypes::POSITIVE:
            return "+";

        case SignOpsTypes::NEGATIVE:
            return "-";
    }

    return "Unknown";
}

// ==================================================================
// Constructor
// ==================================================================

SignOpNode::SignOpNode(
    SignOpsTypes type,
    const Token& token,
    const Token& operand_ft,
    std::unique_ptr<ASTNode>& node
):
    type(type),
    token(token),
    Operand_FT(operand_ft),
    node(std::move(node)) {}

// ==================================================================
// Get string representation
// ==================================================================

std::string
SignOpNode::get_str(int level) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";
    ss << "Sign operation:\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";
    ss << "Type: " << sign_to_string(type) << "\n";

    if (node) {
        for (int i = 0; i < level + 1; i++)
            ss << "|  ";

        ss << "Operand:\n";
        ss << node->get_str(level + 2);
    }

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
SignOpNode::get_json() {
    nlohmann::json result;

    result["type"] = "SignOperation";
    result["operator"] = sign_to_string(type);

    if (node)
        result["operand"] = node->get_json();
    else
        result["operand"] = nullptr;

    return result;
}

// ==================================================================
// Get node type
// ==================================================================

ASTNodesTypes
SignOpNode::NType() {
    return NT__SignOpNode;
}

// ==================================================================
// Clone
// ==================================================================

ASTNode*
SignOpNode::clone() {
    std::unique_ptr<ASTNode> node_copy;

    if (node)
        node_copy.reset(node->clone());

    return new SignOpNode(
        type,
        token,
        Operand_FT,
        node_copy
    );
}

// ==================================================================
// Semantic analysis
// ==================================================================

ReturnResult<bool>
SignOpNode::accept(NodesContext Context) {
    if (!node) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Analysis,
            "Sign operation has no operand",
            token.line
        );

        ReturnResult<bool> re = {
            "Sign operation has no operand",
            false,
            false
        };

        re.error_info = {
            "NodeNotExists",
            "Sign operation has no operand",
            "",
            Operand_FT.line,
            Operand_FT.column,
            Operand_FT.value.size()
        };

        return re;
    }

    emit_sign_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing unary sign '" + sign_to_string(type) + "'",
        token.line
    );

    // Analyze operand
    auto result = node->accept(Context);

    if (!result.success) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Analysis,
            "Sign operation failed while analyzing its operand",
            token.line
        );

        return result;
    }

    // Unary signs accept numeric operands only
    if (
        node->return_type != "int" &&
        node->return_type != "float"
    ) {
        auto error = Errors::TypeError(
            "int | float",
            node->return_type,
            Operand_FT.line,
            Operand_FT.column,
            Operand_FT.value.size(),
            "Unary sign operations support numeric types only"
        );

        emit_sign_event(
            Context,
            DevTools::EventsT::Analysis,
            "Sign operation rejected operand of type '" +
                node->return_type +
                "'; expected 'int' or 'float'",
            Operand_FT.line
        );

        ReturnResult<bool> re = {
            error.msg,
            false,
            false
        };

        re.error_info = error.info;
        return re;
    }

    // Preserve the operand's type
    return_type = node->return_type;

    emit_sign_event(
        Context,
        DevTools::EventsT::Analysis,
        "Unary sign '" + sign_to_string(type) +
            "' passed semantic analysis",
        token.line
    );

    return {"", true, true};
}

// ==================================================================
// Execution
// ==================================================================

ReturnResult<Value>
SignOpNode::exec(NodesContext Context) {
    if (!node) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Analysis,
            "Sign operation has no operand",
            token.line
        );

        ReturnResult<Value> re = {
            "Sign operation has no operand",
            false,
            std::monostate{}
        };

        re.error_info = {
            "NodeNotExists",
            "Sign operation has no operand",
            "",
            Operand_FT.line,
            Operand_FT.column,
            Operand_FT.value.size()
        };

        return re;
    }

    emit_sign_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing unary sign '" + sign_to_string(type) + "'",
        token.line
    );

    // Execute operand
    auto result = node->exec(Context);

    if (!result.success) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Runtime,
            "Sign operation failed while evaluating its operand",
            token.line
        );

        return result;
    }

    // Get the value
    auto value_r = turn_value_to_float(result.value);
    if (!value_r.success) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Runtime,
            "Unary ("+sign_to_string(type)+") received a non-numeric operand",
            token.line
        );
        auto error = Errors::TypeError(
            "int | float",
            node->return_type,
            Operand_FT.line,
            Operand_FT.column,
            Operand_FT.value.size(),
            "Unary sign operations support numeric types only"
        );
        ReturnResult<Value> re = {
            error.msg,
            false,
            std::monostate{}
        };
        re.error_info = error.info;
        return re;
    }

    // Positive sign: preserve the numeric value and type
    if (type == SignOpsTypes::POSITIVE) {

        emit_sign_event(
            Context,
            DevTools::EventsT::Runtime,
            "Unary '+' completed successfully",
            token.line
        );

        auto v = Value(value_r.value);
        return {"", true, reconsiliation_int_float(node->return_type,v)};
    
    } 

    // Negative sign
    auto negative = Value(-value_r.value);

    emit_sign_event(
        Context,
        DevTools::EventsT::Runtime,
        "Unary '-' completed successfully",
        token.line
    );
    
    return {"", true, reconsiliation_int_float(node->return_type,negative)};
}


// ==================================================================
// Logical NOT Node
// ==================================================================

// Constructor
LogicalNotNode::LogicalNotNode(
    const Token& token,
    const Token& operand_token,
    std::unique_ptr<ASTNode>& node
)
    : token(token),
      Operand_FT(operand_token),
      node(std::move(node)) {}

// Get string representation
std::string
LogicalNotNode::get_str(int level) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";
    ss << "Logical NOT operation:\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";
    ss << "Operator !\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";
    ss << "Operand:\n" << node->get_str(level + 2);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
LogicalNotNode::get_json() {
    nlohmann::json result;

    result["type"] = "LogicalNot";
    result["operator"] = "!";

    result["operand"] = node->get_json();

    return result;
}

// Get node type
ASTNodesTypes 
LogicalNotNode::NType() {
    return NT__LogicalNotNode;
}

// Semantic analysis
ReturnResult<bool> 
LogicalNotNode::accept(
    NodesContext Context
) {
    // Analyze the operand
    auto result = node->accept(Context);
    if (!result.success) return result;
    

    // Logical NOT only supports boolean operands
    if (node->return_type != "bool") {
        auto error_info = Errors::TypeError(
            "bool",
            node->return_type,
            token.line,
            token.column,
            token.value.size(),
            "Logical NOT supports boolean operands only"
        );

        return {"", false, false};
    }

    // The result of logical NOT is always boolean
    return_type = "bool";

    return {"", true, true};
}

// Execution
ReturnResult<Value> LogicalNotNode::exec(NodesContext Context) {
    // Execute the operand
    auto result = node->exec(Context);

    if (!result.success) {
        return result;
    }

    return {"",true,result.value != Value(true)};
    
}

// Clone
ASTNode* LogicalNotNode::clone() {
    std::unique_ptr<ASTNode> new_node(node->clone());
    return new LogicalNotNode(
        token,
        Operand_FT,
        new_node
    );
}
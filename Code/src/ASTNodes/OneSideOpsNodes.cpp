
// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "detail/globals.hpp"
#include "detail/ast/one_side_ops_nodes.hpp"
#include "detail/errors.hpp"

// == Libs ==
#include <sstream>
#include <limits>

// ==================================================================
// Helper: emit a Runtime Tracker event
// ==================================================================

namespace vltc {
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
            line,
            Context.ParentScope,
            Context.FilePath.string()
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
    const Token& END_token,
    SignOpsTypes type,
    const Token& token,
    const Token& operand_ft,
    std::unique_ptr<ASTNode>& node
):
    type(type),
    token(token),
    Operand_FT(operand_ft),
    node(std::move(node)),
    ASTNode(END_token) {}

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

    return new SignOpNode(END_token, type,
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
            DevTools::EventsT::Error,
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
            Operand_FT.FilePath,
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
            DevTools::EventsT::Error,
            "Fail in analyzing operand of unary sign: " + result.Message,
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
            DevTools::EventsT::Error,
            error.msg,
            Operand_FT.line
        );

        ReturnResult<bool> re = {
            error.msg,
            false,
            false
        };

        re.error_info = error.info;
        re.error_info.file = Operand_FT.FilePath;
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
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_sign_event(Context, DevTools::EventsT::Runtime,
        "Executing unary sign '" + sign_to_string(type) + "'",
        token.line ? token.line : END_token.line);
    if (!node) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Error,
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
            Operand_FT.FilePath,
            "Sign operation has no operand",
            "",
            Operand_FT.line,
            Operand_FT.column,
            Operand_FT.value.size()
        };

        return re;
    }

    if (type == SignOpsTypes::NEGATIVE &&
        Operand_FT.Type == TokenType::INTEGER &&
        Operand_FT.value == "9223372036854775808") {
        return {"", true, std::numeric_limits<int64_t>::min()};
    }

    // Execute operand
    auto result = node->exec(Context);

    if (!result.success) {
        emit_sign_event(
            Context,
            DevTools::EventsT::Error,
            "Fail in evaluating operand of unary sign: " + result.Message,
            token.line
        );

        return result;
    }

    // Get the value
    auto value_r = turn_value_to_float(result.value);
    if (!value_r.success) {
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
            DevTools::EventsT::Error,
            error.msg,
            token.line
        );
        ReturnResult<Value> re = {
            error.msg,
            false,
            std::monostate{}
        };
        re.error_info = error.info;
        re.error_info.file = Operand_FT.FilePath;
        return re;
    }

    // Positive sign: preserve the numeric value and type
    if (type == SignOpsTypes::POSITIVE) {

        emit_sign_event(
            Context,
            DevTools::EventsT::Info,
            "Unary '+' completed successfully",
            token.line
        );

        auto v = Value(value_r.value);
        auto reconciled = reconsiliation_int_float(
            node->return_type, v, Operand_FT
        );
        if (!reconciled.success) {
            emit_sign_event(Context, DevTools::EventsT::Error,
                "Fail in reconciling result of unary sign: " + reconciled.Message,
                Operand_FT.line);
            return reconciled;
        }
        return {"", true, std::move(reconciled.value)};

    }

    // Negative sign
    auto negative = Value(-value_r.value);

    emit_sign_event(
        Context,
        DevTools::EventsT::Info,
        "Unary '-' completed successfully",
        token.line
    );

    auto reconciled = reconsiliation_int_float(
        node->return_type, negative, Operand_FT
    );
    if (!reconciled.success) {
        emit_sign_event(Context, DevTools::EventsT::Error,
            "Fail in reconciling result of unary sign: " + reconciled.Message,
            Operand_FT.line);
        return reconciled;
    }
    return {"", true, std::move(reconciled.value)};
}


// ==================================================================
// Logical NOT Node
// ==================================================================

// Constructor
LogicalNotNode::LogicalNotNode(
    const Token& END_token,
    const Token& token,
    const Token& operand_token,
    std::unique_ptr<ASTNode>& node
)
    : token(token),
      Operand_FT(operand_token),
      node(std::move(node)),
    ASTNode(END_token) {}

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
    emit_sign_event(Context, DevTools::EventsT::Analysis,
        "Analyzing logical NOT", token.line ? token.line : END_token.line);
    // Analyze the operand
    auto result = node->accept(Context);
    if (!result.success) {
        emit_sign_event(Context, DevTools::EventsT::Error,
            "Fail in analyzing operand of logical NOT: " + result.Message,
            token.line ? token.line : END_token.line);
        return result;
    }


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
        error_info.info.file = token.FilePath;
        emit_sign_event(Context, DevTools::EventsT::Error,
            error_info.info.message, token.line);

        return {"", false, false};
    }

    // The result of logical NOT is always boolean
    return_type = "bool";

    emit_sign_event(Context, DevTools::EventsT::Analysis,
        "Logical NOT passed semantic analysis", token.line);

    return {"", true, true};
}

// Execution
ReturnResult<Value> LogicalNotNode::exec(NodesContext Context) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_sign_event(Context, DevTools::EventsT::Runtime,
        "Executing logical NOT", token.line ? token.line : END_token.line);
    // Execute the operand
    auto result = node->exec(Context);

    if (!result.success) {
        emit_sign_event(Context, DevTools::EventsT::Error,
            "Fail in evaluating operand of logical NOT: " + result.Message,
            token.line ? token.line : END_token.line);
        return result;
    }

    return {"",true,result.value != Value(true)};

}

// Clone
ASTNode* LogicalNotNode::clone() {
    std::unique_ptr<ASTNode> new_node(node->clone());
    return new LogicalNotNode(END_token, token,
        Operand_FT,
        new_node
    );
}

} // namespace vltc

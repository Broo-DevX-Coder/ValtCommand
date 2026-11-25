// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "detail/globals.hpp"
#include "detail/ast/value_node.hpp"
#include "detail/errors.hpp"

// == Libs ==
#include <sstream>
#include <string>
#include <cstdint>

// ==================================================================
// Runtime Tracker Helper
// ==================================================================

namespace vltc {
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
            line,
            Context.ParentScope,
            Context.FilePath.string()
        });
    }
}

// ==================================================================
// Main Value Node Functions
// ==================================================================

// Constructor
ValueNode::ValueNode(
    const Token& END_token,
    Token t_,
    std::string tt_
):
    vToken(t_),
    type(tt_),
    ASTNode(END_token) {
    return_type = type;
}

// Get string of node to print
std::string
ValueNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Value:\n";

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "| Type: " << type << "\n";

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "| Value: " << vToken.value << "\n";

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
ValueNode::get_json() {
    nlohmann::json result;

    result["type"] = "Value";
    result["value_type"] = type;
    result["value"] = vToken.value;

    return result;
}

// Get node type
ASTNodesTypes
ValueNode::NType() {
    return NT__ValueNode;
}

// ==================================================================
// String Value Node
// ==================================================================

StringValueNode::StringValueNode(
    const Token& END_token,
    Token t_
): ValueNode(END_token, t_, "str") {}

// Semantic analysis
ReturnResult<bool>
StringValueNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing string literal",
        vToken.line
    );

    if (vToken.Type != TokenType::STRING) {
        auto err = Errors::TypeError(
            type,
            TokenTypes_to_StringType[vToken.Type],
            vToken.line,
            vToken.column,
            vToken.value.size()
        );
        emit_event(Context, DevTools::EventsT::Error, err.msg, vToken.line);

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;
        re.error_info.file = vToken.FilePath;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "String literal validated successfully",
        vToken.line
    );

    return {"", true, true};
}

// Runtime execution
ReturnResult<Value>
StringValueNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing string literal",
        vToken.line
    );

    value = vToken.value;

    emit_event(
        Context,
        DevTools::EventsT::Debug,
        "String literal evaluated successfully",
        vToken.line
    );

    return {"", true, std::move(value)};
}

// Get node type
ASTNodesTypes
StringValueNode::NType() {
    return NT__StringValueNode;
}

// Clone node
ASTNode*
StringValueNode::clone() {
    return new StringValueNode(END_token, vToken);
}

// ==================================================================
// Number Value Node
// ==================================================================

NumberValueNode::NumberValueNode(
    const Token& END_token,
    Token t_,
    std::string tt_
): ValueNode(END_token, t_, tt_) {}

// Semantic analysis
ReturnResult<bool>
NumberValueNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing numeric literal",
        vToken.line
    );

    if (
        vToken.Type != TokenType::INTEGER &&
        vToken.Type != TokenType::FLOAT
    ) {
        auto err = Errors::TypeError(
            type,
            TokenTypes_to_StringType[vToken.Type],
            vToken.line,
            vToken.column,
            vToken.value.size()
        );
        emit_event(Context, DevTools::EventsT::Error, err.msg, vToken.line);

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;
        re.error_info.file = vToken.FilePath;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Numeric literal validated successfully",
        vToken.line
    );

    return {"", true, true};
}

// Runtime execution
ReturnResult<Value>
NumberValueNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing numeric literal",
        vToken.line
    );

    try {
        auto v = std::stold(vToken.value);

        if (type == "int") {
            value = static_cast<int64_t>(v);
        } else {
            value = static_cast<long double>(v);
        }
    }
    catch (const std::exception& e) {
        const std::string message =
            std::string("Numeric literal conversion failed: ") + e.what();
        emit_event(
            Context,
            DevTools::EventsT::Error,
            message,
            vToken.line
        );

        return {
            message,
            false,
            std::monostate{}
        };
    }

    emit_event(
        Context,
        DevTools::EventsT::Debug,
        "Numeric literal evaluated successfully",
        vToken.line
    );

    return {"", true, std::move(value)};
}

// Get node type
ASTNodesTypes
NumberValueNode::NType() {
    return NT__NumberValueNode;
}

// Clone node
ASTNode*
NumberValueNode::clone() {
    return new NumberValueNode(END_token, vToken, type);
}

// ==================================================================
// Boolean Value Node
// ==================================================================

BooleanValueNode::BooleanValueNode(
    const Token& END_token,
    Token t_
): ValueNode(END_token, t_, "bool") {}

// Semantic analysis
ReturnResult<bool>
BooleanValueNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing boolean literal",
        vToken.line
    );

    if (vToken.Type != TokenType::BOOLEAN) {
        auto err = Errors::TypeError(
            type,
            TokenTypes_to_StringType[vToken.Type],
            vToken.line,
            vToken.column,
            vToken.value.size()
        );
        emit_event(Context, DevTools::EventsT::Error, err.msg, vToken.line);

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;
        re.error_info.file = vToken.FilePath;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Boolean literal validated successfully",
        vToken.line
    );

    return {"", true, true};
}

// Runtime execution
ReturnResult<Value>
BooleanValueNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing boolean literal",
        vToken.line
    );

    value = vToken.value == "True";

    emit_event(
        Context,
        DevTools::EventsT::Debug,
        "Boolean literal evaluated successfully",
        vToken.line
    );

    return {"", true, std::move(value)};
}

// Get node type
ASTNodesTypes
BooleanValueNode::NType() {
    return NT__BooleanValueNode;
}

// Clone node
ASTNode*
BooleanValueNode::clone() {
    return new BooleanValueNode(END_token, vToken);
}

} // namespace vltc

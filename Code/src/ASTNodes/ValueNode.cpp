// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/ValueNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>
#include <string>
#include <cstdint>

// ==================================================================
// Runtime Tracker Helper
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
// Main Value Node Functions
// ==================================================================

// Constructor
ValueNode::ValueNode(
    Token t_,
    std::string tt_
):
    vToken(t_),
    type(tt_)
{
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

// Get node type
ASTNodesTypes
ValueNode::NType() {
    return NT__ValueNode;
}

// ==================================================================
// String Value Node
// ==================================================================

StringValueNode::StringValueNode(
    Token t_
): ValueNode(t_, "str") {}

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
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "String literal type validation failed",
            vToken.line
        );

        auto err = Errors::TypeError(
            type,
            TokenTypes_to_StringType[vToken.Type],
            vToken.line,
            vToken.column,
            vToken.value.size()
        );

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;

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
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Evaluating string literal",
        vToken.line
    );

    value = vToken.value;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
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
    return new StringValueNode(vToken);
}

// ==================================================================
// Number Value Node
// ==================================================================

NumberValueNode::NumberValueNode(
    Token t_,
    std::string tt_
): ValueNode(t_, tt_) {}

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
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Numeric literal type validation failed",
            vToken.line
        );

        auto err = Errors::TypeError(
            type,
            TokenTypes_to_StringType[vToken.Type],
            vToken.line,
            vToken.column,
            vToken.value.size()
        );

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;

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
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Evaluating numeric literal",
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
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Numeric literal conversion failed",
            vToken.line
        );

        return {
            std::string("Numeric literal conversion failed: ") + e.what(),
            false,
            std::monostate{}
        };
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
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
    return new NumberValueNode(vToken, type);
}

// ==================================================================
// Boolean Value Node
// ==================================================================

BooleanValueNode::BooleanValueNode(
    Token t_
): ValueNode(t_, "bool") {}

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
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Boolean literal type validation failed",
            vToken.line
        );

        auto err = Errors::TypeError(
            type,
            TokenTypes_to_StringType[vToken.Type],
            vToken.line,
            vToken.column,
            vToken.value.size()
        );

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;

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
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Evaluating boolean literal",
        vToken.line
    );

    value = vToken.value == "True";

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
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
    return new BooleanValueNode(vToken);
}
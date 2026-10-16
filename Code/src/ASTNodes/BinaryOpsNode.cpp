// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/BinaryOpsNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>
#include <string>

// ==================================================================
// Comparison symbols to operations
// ==================================================================

std::unordered_map<TokenType, ComparitonOpsTypes> ComparitonSymbols_ToOps = {
    {TokenType::EQUAL_EQUAL,    ComparitonOpsTypes::EQUAL},
    {TokenType::NOT_EQUAL,      ComparitonOpsTypes::NOT_EQUAL},
    {TokenType::LESS_THAN,      ComparitonOpsTypes::LESS_THEN},
    {TokenType::GREATER_THAN,   ComparitonOpsTypes::GREATER_THEN},
    {TokenType::LESS_EQUAL,     ComparitonOpsTypes::LESS_THEN_OR_EQUAL},
    {TokenType::GREATER_EQUAL,  ComparitonOpsTypes::GREATER_THEN_OR_EQUAL}
};

// ==================================================================
// Helper: emit a Runtime Tracker event
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
// Helper: convert comparison operation to string
// ==================================================================

static std::string comparison_to_string(ComparitonOpsTypes type) {
    switch (type) {
        case ComparitonOpsTypes::EQUAL:
            return "==";

        case ComparitonOpsTypes::NOT_EQUAL:
            return "!=";

        case ComparitonOpsTypes::LESS_THEN:
            return "<";

        case ComparitonOpsTypes::GREATER_THEN:
            return ">";

        case ComparitonOpsTypes::LESS_THEN_OR_EQUAL:
            return "<=";

        case ComparitonOpsTypes::GREATER_THEN_OR_EQUAL:
            return ">=";
    }

    return "Unknown";
}

// ==================================================================
// Helper: convert binary operation to string
// ==================================================================

static std::string binary_to_string(TokenType op) {
    switch (op) {
        case TokenType::PLUS:
            return "+";

        case TokenType::MINUS:
            return "-";

        case TokenType::STAR:
            return "*";

        case TokenType::SLASH:
            return "/";

        case TokenType::DIV:
            return "DIV";
        
        case TokenType::MOD:
            return "MOD";

        default:
            return "Unknown";
    }
}

// ==================================================================
// Binary operation node
// ==================================================================

// Constructor
BinOpsNode::BinOpsNode(
    OperationPartsList& parts_
):
    Parts(std::move(parts_)) {}

// Get string representation
std::string
BinOpsNode::get_str(int level) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Binary operation:\n";

    size_t part_i = 0;
    for (auto& part : Parts) {
        for (int i = 0; i < level + 1; i++)
            ss << "|  ";
        
        if (part_i == 0)
            ss << "First" << ":\n";
        else
            ss << binary_to_string(part.op) << ":\n";

        ss << part.node->get_str(level + 2);
        part_i++;
    }

    return ss.str();
}

// Get node type
ASTNodesTypes
BinOpsNode::NType() {
    return NT__BinOpsNode;
}

// ==================================================================
// Binary operation: semantic analysis
// ==================================================================

ReturnResult<bool>
BinOpsNode::accept(NodesContext Context) {
    return_type = "float";

    if (!Parts.empty()) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing binary operation with " +
                std::to_string(Parts.size()) + " operand(s)",
            Parts.front().token.line
        );
    }

    for (auto& part : Parts) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Checking operand type for binary operator '" +
                binary_to_string(part.op) + "'",
            part.token.line
        );

        auto accept_result = part.node->accept(Context);

        if (!accept_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Binary operation analysis failed while checking an operand",
                part.token.line
            );

            return accept_result;
        }

        if (
            part.node->return_type != "int" &&
            part.node->return_type != "float"
        ) {
            auto e = Errors::TypeError(
                "int | float",
                part.node->return_type,
                part.token.line,
                part.token.column,
                part.token.value.size(),
                "This operation does not support types other than int or float"
            );

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Binary operation rejected operand of type '" +
                    part.node->return_type +
                    "'; expected 'int' or 'float'",
                part.token.line
            );

            ReturnResult<bool> re = {e.msg, false, false};
            re.error_info = e.info;
            return re;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Operand type '" + part.node->return_type +
                "' is valid for binary operation",
            part.token.line
        );
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Binary operation analysis completed successfully",
        Parts.empty() ? 0 : Parts.back().token.line
    );

    return {"", true, true};
}

// ==================================================================
// Apply binary operation
// ==================================================================

void
BinOpsNode::push_to_result(
    long double& result,
    TokenType op,
    long double input
) {
    if (op == TokenType::PLUS) {
        result += input;
    } else if (op == TokenType::MINUS) {
        result -= input;
    } else if (op == TokenType::STAR) {
        result *= input;
    } else if (op == TokenType::SLASH) {
        result /= input;
    } else if (op == TokenType::DIV) {
        int64_t r = result / input;
        result = r;
    } else if (op == TokenType::MOD) {
        int64_t i1 = result;
        int64_t i2 = input;
        result = i1 % i2;
    }
}

// ==================================================================
// Binary operation: execution
// ==================================================================

ReturnResult<Value>
BinOpsNode::exec(NodesContext Context) {
    return_type = "float";

    if (Parts.empty()) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Binary operation contains no operands",
            0
        );

        return {"Binary operation contains no operands", false, 0};
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing binary operation",
        Parts.front().token.line
    );

    long double result = 0;

    for (size_t i = 0; i < Parts.size(); i++) {
        auto& part = Parts[i];

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Evaluating operand for operator '" +
                binary_to_string(part.op) + "'",
            part.token.line
        );

        auto exec_result = part.node->exec(Context);

        if (!exec_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Binary operation failed while evaluating an operand",
                part.token.line
            );

            return exec_result;
        }

        auto val = turn_value_to_float(exec_result.value);

        // Division by zero
        if ((part.op == TokenType::SLASH || part.op == TokenType::DIV || part.op == TokenType::MOD) && val.value == 0) {
            Errors::RuntimeError error(
                part.token.line,
                part.token.column,
                part.token.value.size()
            );

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Division by zero detected",
                part.token.line
            );

            ReturnResult<Value> re = {
                error.division_by_zero(),
                false,
                0
            };

            re.error_info = error.info;
            return re;
        }

        // Apply operation
        if (i == 0) {
            result = val.value;
        } else {
            push_to_result(result, part.op, val.value);
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Applied operator '" + binary_to_string(part.op) + "'",
            part.token.line
        );
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Binary operation completed successfully",
        Parts.back().token.line
    );

    return {"", true, result};
}

// ==================================================================
// Clone binary operation node
// ==================================================================

ASTNode*
BinOpsNode::clone() {
    OperationPartsList new_parts;

    for (auto& p : Parts) {
        std::unique_ptr<ASTNode> value_node(p.node->clone());

        new_parts.push_back({
            p.op,
            p.token,
            std::move(value_node)
        });
    }

    return new BinOpsNode(new_parts);
}

// ==================================================================
// Comparison operation node
// ==================================================================

// Constructor
CompOpsNode::CompOpsNode(
    OperationPart& FToken,
    OperationPart& SToken,
    ComparitonOpsTypes T
):
    first_part(std::move(FToken)),
    second_part(std::move(SToken)),
    type(T) {}

// Get string representation
std::string
CompOpsNode::get_str(int level) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Comparison operation:\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Type: " << comparison_to_string(type) << "\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "First part:\n"
       << first_part.node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Second part:\n"
       << second_part.node->get_str(level + 2);

    return ss.str();
}

// Get node type
ASTNodesTypes
CompOpsNode::NType() {
    return NT__CompOpsNode;
}

// ==================================================================
// Clone comparison node
// ==================================================================

ASTNode*
CompOpsNode::clone() {
    std::unique_ptr<ASTNode> fpart_ptr(first_part.node->clone());
    std::unique_ptr<ASTNode> spart_ptr(second_part.node->clone());

    OperationPart FirstP = {
        TokenType::UNKNOWN,
        first_part.token,
        std::move(fpart_ptr)
    };

    OperationPart SecondP = {
        TokenType::UNKNOWN,
        second_part.token,
        std::move(spart_ptr)
    };

    return new CompOpsNode(FirstP, SecondP, type);
}

// ==================================================================
// Comparison: semantic analysis
// ==================================================================

ReturnResult<bool>
CompOpsNode::accept(NodesContext Context) {
    return_type = "bool";

    const std::string op = comparison_to_string(type);

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing comparison operation '" + op + "'",
        first_part.token.line
    );

    // Analyze first operand
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing left operand of '" + op + "'",
        first_part.token.line
    );

    auto fresult = first_part.node->accept(Context);

    if (!fresult.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Left operand analysis failed",
            first_part.token.line
        );

        return fresult;
    }

    // Analyze second operand
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing right operand of '" + op + "'",
        second_part.token.line
    );

    auto sresult = second_part.node->accept(Context);

    if (!sresult.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Right operand analysis failed",
            second_part.token.line
        );

        return sresult;
    }

    const auto& first_type = first_part.node->return_type;
    const auto& second_type = second_part.node->return_type;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Checking comparison compatibility between '" +
            first_type + "' and '" + second_type + "'",
        second_part.token.line
    );

    // Check compatible types
    if (!are_types_compatible(first_type, second_type)) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Comparison rejected: incompatible operand types '" +
                first_type + "' and '" + second_type + "'",
            second_part.token.line
        );

        return {
            fmt::format(
                "UncompatibleTypesError: can't compare `{}` with `{}` at line:{} ,column:{}",
                first_type,
                second_type,
                second_part.token.line,
                second_part.token.column
            ),
            false,
            false
        };
    }

    // Ordered comparisons require compatible numeric types
    const bool is_ordered_comparison =
        type == ComparitonOpsTypes::LESS_THEN ||
        type == ComparitonOpsTypes::GREATER_THEN ||
        type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL ||
        type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL;

    const bool both_numeric =
        (first_type == "int" || first_type == "float") &&
        (second_type == "int" || second_type == "float");

    if (
        is_ordered_comparison &&
        !both_numeric
    ) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Ordered comparison '" + op +
                "' requires numeric operands; received '" +
                first_type + "' and '" + second_type + "'",
            second_part.token.line
        );

        return {
            fmt::format(
                "UncompatibleTypesError: can't compare `{}` with `{}` at line:{} ,column:{}\nThese types don't support this comparison operation",
                first_type,
                second_type,
                second_part.token.line,
                second_part.token.column
            ),
            false,
            false
        };
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Comparison operation '" + op +
            "' passed semantic analysis",
        second_part.token.line
    );

    return {"", true, true};
}

// ==================================================================
// Comparison: execution
// ==================================================================

ReturnResult<Value>
CompOpsNode::exec(NodesContext Context) {
    return_type = "bool";

    const std::string op = comparison_to_string(type);

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing comparison '" + op + "'",
        first_part.token.line
    );

    // Execute operands
    auto fresult = first_part.node->exec(Context);

    if (!fresult.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Comparison failed while evaluating the left operand",
            first_part.token.line
        );

        return fresult;
    }

    auto sresult = second_part.node->exec(Context);

    if (!sresult.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Comparison failed while evaluating the right operand",
            second_part.token.line
        );

        return sresult;
    }

    const auto FN_return_t = first_part.node->return_type;
    const auto SN_return_t = second_part.node->return_type;

    // Do not set child AST pointers to nullptr here.
    // They remain owned by this node and may be needed later.

    Value cp_result;

    // Equality comparisons
    if (type == ComparitonOpsTypes::EQUAL) {
        cp_result = fresult.value == sresult.value;
    } else if (type == ComparitonOpsTypes::NOT_EQUAL) {
        cp_result = fresult.value != sresult.value;
    }

    // Numeric comparisons
    if (
        (FN_return_t == "int" || FN_return_t == "float") &&
        (SN_return_t == "int" || SN_return_t == "float")
    ) {
        const auto fv = turn_value_to_float(fresult.value).value;
        const auto sv = turn_value_to_float(sresult.value).value;

        if (type == ComparitonOpsTypes::EQUAL) {
            cp_result = fv == sv;
        } else if (type == ComparitonOpsTypes::NOT_EQUAL) {
            cp_result = fv != sv;
        } else if (type == ComparitonOpsTypes::LESS_THEN) {
            cp_result = fv < sv;
        } else if (type == ComparitonOpsTypes::GREATER_THEN) {
            cp_result = fv > sv;
        } else if (type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL) {
            cp_result = fv <= sv;
        } else if (type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL) {
            cp_result = fv >= sv;
        }
    } else if (
        type == ComparitonOpsTypes::LESS_THEN ||
        type == ComparitonOpsTypes::GREATER_THEN ||
        type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL ||
        type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL
    ) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Cannot execute ordered comparison with non-numeric operands",
            second_part.token.line
        );

        return {
            "Invalid operand types for ordered comparison",
            false,
            false
        };
    }

    // Report comparison result
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Comparison '" + op + "' evaluated successfully",
        second_part.token.line
    );

    return {"", true, cp_result};
}

// ==================================================================
// Logical operation node
// ==================================================================

// Constructor
LogicOpsNode::LogicOpsNode(
    OperationPartsList& parts
):
    Parts(std::move(parts)) {}

// Get string representation
std::string
LogicOpsNode::get_str(int level) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Logical operation:\n";

    for (auto& part : Parts) {
        for (int i = 0; i < level + 1; i++)
            ss << "|  ";

        if (part.op == TokenType::LOGICAL_AND)
            ss << "AND &&:\n";
        else if (part.op == TokenType::LOGICAL_OR)
            ss << "OR ||:\n";
        else
            ss << "First:\n";

        ss << part.node->get_str(level + 2);
    }

    return ss.str();
}

// Get node type
ASTNodesTypes
LogicOpsNode::NType() {
    return NT__LogicOpsNode;
}

// ==================================================================
// Clone logical operation node
// ==================================================================

ASTNode*
LogicOpsNode::clone() {
    OperationPartsList new_parts;

    for (auto& p : Parts) {
        std::unique_ptr<ASTNode> value_node(p.node->clone());

        new_parts.push_back({
            p.op,
            p.token,
            std::move(value_node)
        });
    }

    return new LogicOpsNode(new_parts);
}

// ==================================================================
// Logical operation: semantic analysis
// ==================================================================

ReturnResult<bool>
LogicOpsNode::accept(NodesContext Context) {
    return_type = "bool";

    if (Parts.empty()) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Logical operation contains no operands",
            0
        );

        return {"Logical operation contains no operands", false, false};
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing logical operation with " +
            std::to_string(Parts.size()) + " operand(s)",
        Parts.front().token.line
    );

    for (auto& part : Parts) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Checking logical operand; expected type 'bool'",
            part.token.line
        );

        auto r = part.node->accept(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Logical operand analysis failed",
                part.token.line
            );

            return r;
        }

        if (part.node->return_type != "bool") {
            auto err = Errors::TypeError(
                "bool",
                part.node->return_type,
                part.token.line,
                part.token.column,
                part.token.value.size()
            );

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Logical operation rejected operand of type '" +
                    part.node->return_type + "'; expected 'bool'",
                part.token.line
            );

            ReturnResult<bool> re = {err.msg, false, false};
            re.error_info = err.info;

            return re;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Logical operation passed semantic analysis",
        Parts.back().token.line
    );

    return {"", true, true};
}

// ==================================================================
// Logical operation: execution
// ==================================================================

ReturnResult<Value>
LogicOpsNode::exec(NodesContext Context) {
    return_type = "bool";

    if (Parts.empty()) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Logical operation contains no operands",
            0
        );

        return {"Logical operation contains no operands", false, false};
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing logical operation",
        Parts.front().token.line
    );

    // Execute first operand
    auto r = Parts[0].node->exec(Context);

    if (!r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Logical operation failed while evaluating the first operand",
            Parts[0].token.line
        );

        return r;
    }

    auto* first_value = std::get_if<bool>(&r.value);

    if (!first_value) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Logical operation received a non-boolean first operand",
            Parts[0].token.line
        );

        return {"Expected bool from logical operand", false, false};
    }

    bool result = *first_value;

    // Execute remaining operands
    for (size_t i = 1; i < Parts.size(); i++) {
        auto& part = Parts[i];

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Evaluating logical operator '" +
                std::string(
                    part.op == TokenType::LOGICAL_AND ? "&&" :
                    part.op == TokenType::LOGICAL_OR ? "||" : "?"
                ) + "'",
            part.token.line
        );

        auto part_result = part.node->exec(Context);

        if (!part_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Logical operation failed while evaluating an operand",
                part.token.line
            );

            return part_result;
        }

        auto* part_value = std::get_if<bool>(&part_result.value);

        if (!part_value) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Logical operation received a non-boolean operand",
                part.token.line
            );

            return {"Expected bool from logical operand", false, false};
        }

        const bool part_r = *part_value;

        if (part.op == TokenType::LOGICAL_AND) {
            result = result && part_r;
        } else if (part.op == TokenType::LOGICAL_OR) {
            result = result || part_r;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Logical operation completed successfully",
        Parts.back().token.line
    );

    return {"", true, result};
}

// ==================================================================
// Constructor
// ==================================================================


// Constructor
PowerNode::PowerNode(
    const Token& token,
    OperationPart& left,
    OperationPart& right
)
    : token(token),
      left(std::move(left)),
      right(std::move(right)) {}

// Get string representation
std::string
PowerNode::get_str(int level) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Power operation:\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Operator **:\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Base:\n";
    ss << left.node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Exponent:\n";
    ss << right.node->get_str(level + 2);

    return ss.str();
}

// Get node type
ASTNodesTypes PowerNode::NType() {
    return NT__PowerNode;
}

// Semantic analysis
ReturnResult<bool> 
PowerNode::accept(
    NodesContext Context
) {
    // Analyze the base
    auto left_result = left.node->accept(Context);
    if (!left_result.success) return left_result;

    // Analyze the exponent
    auto right_result = right.node->accept(Context);
    if (!right_result.success) return right_result;

    // Validate the base type
    if (left.node->return_type != "int" &&
        left.node->return_type != "float") {

        auto error = Errors::TypeError(
            "int | float",
            left.node->return_type,
            left.token.line,
            left.token.column,
            left.token.value.size(),
            "The base of a power operation must be numeric"
        );

        ReturnResult<bool> re = {error.msg, false, false};
        re.error_info = error.info;

        return re;
    }

    // Validate the exponent type
    if (right.node->return_type != "int" &&
        right.node->return_type != "float") {

        auto error = Errors::TypeError(
            "int | float",
            right.node->return_type,
            right.token.line,
            right.token.column,
            right.token.value.size(),
            "The exponent of a power operation must be numeric"
        );

        ReturnResult<bool> re = {error.msg, false, false};
        re.error_info = error.info;

        return re;
    }

    // Power operations return float
    return_type = "float";

    return {"", true, true};
}

// Execution
ReturnResult<Value> 
PowerNode::exec
(
    NodesContext Context
) {
    // Execute the base
    auto left_result = left.node->exec(Context);
    if (!left_result.success) return left_result;

    // Execute the exponent
    auto right_result = right.node->exec(Context);
    if (!right_result.success) return right_result;

    // Convert the base to float
    auto base_result = turn_value_to_float(left_result.value);

    // Convert the exponent to float
    auto exponent_result = turn_value_to_float(right_result.value);

    // Calculate the power
    const long double result = std::pow(
        base_result.value,
        exponent_result.value
    );

    // Reject invalid mathematical results
    if (std::isnan(result)) {
        Errors::ErrorInfo err_info = {
            "ValueError",
            "invalid result in power operation",
            "", 
            left.token.line,
            left.token.column,
            left.token.value.size()
        };
        ReturnResult<Value> re = {"", false, std::monostate{}};
        re.error_info = err_info;
        return re;
    }

    // Reject overflow to infinity
    if (std::isinf(result)) {
        Errors::ErrorInfo err_info = {
            "OverflowError",
            "power operation result is out of range",
            "", 
            left.token.line,
            left.token.column,
            left.token.value.size()
        };
        ReturnResult<Value> re = {"", false, std::monostate{}};
        re.error_info = err_info;
        return re;
    }

    return {"", true, Value{result}};
}

// Clone
ASTNode* PowerNode::clone() {
    OperationPart left_copy{
        left.op,
        left.token,
        std::unique_ptr<ASTNode>(left.node->clone())
    };

    OperationPart right_copy{
        right.op,
        right.token,
        std::unique_ptr<ASTNode>(right.node->clone())
    };

    return new PowerNode(token, left_copy, right_copy);
}
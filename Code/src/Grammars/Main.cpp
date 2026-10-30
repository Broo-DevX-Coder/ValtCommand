// ==================================================================
// Include necessary headers
// ==================================================================
#include "Parser.hpp"
#include "Errors.hpp"

// ==================================================================
// AST Nodes
// ==================================================================
#include "ASTNodes/BinaryOpsNode.hpp"
#include "ASTNodes/OneSideOpsNodes.hpp"

// ==================================================================
// Libraries
// ==================================================================
#include <utility>

// ==================================================================
// Helper: create a parser error result from another result
// ==================================================================

template <typename T>
static ReturnResult<Parser::Node> propagate_error(
    const ReturnResult<T>& result
) {
    ReturnResult<Parser::Node> error = {
        result.Message,
        false,
        nullptr
    };

    error.error_info = result.error_info;
    return error;
}

ReturnResult<Parser::Node>
Parser::get_primary(const std::string& FilePath) {

    auto cu = curent(); // Get curent token

    // Create error object
    auto err = Errors::SyntaxError(
        curent().value,
        curent().line,
        curent().column,
        curent().value.size()
    );

    // Set result var
    ReturnResult<Parser::Node> result = {err.msg, false, nullptr};
    result.error_info = err.info;

    // Set match result var
    ReturnResult<bool> mt;

    // If the curent token is key word (like CALL and SET)
    if (curent().Type == TokenType::KEY_WORD) {

        // Function call node
        if (curent().value == "CALL") {
            result = get_functioncall_node(FilePath);

        // Set or reset variable node
        } else if (curent().value == "SET" || curent().value == "SET_CONST") {
            result = get_set_variable_node(FilePath, curent().value == "SET_CONST");

        // Get variable's value node
        } else if (curent().value == "GET") {
            result = get_get_variable_node(FilePath);

        // Get user function
        } else if (curent().value == "FUNCTION") {
            result = get_user_function_node(FilePath);

        // Get return node
        } else if (curent().value == "RETURN") {
            result = get_return_noode(FilePath);

        // Get If statement node
        } else if (curent().value == "IF") {
            result = get_if_statment_node(FilePath);

        // Get the while loop node
        } else if (curent().value == "WHILE") {
            result = get_while_loop_node(FilePath);

        // Get the for loop node
        } else if (curent().value == "FOR") {
            result = get_for_loop_node(FilePath);

        // Get the break a loop node
        } else if (curent().value == "BREAK") {
            result = get_break_loop_node(FilePath);

        // Get the continue a loop node
        } else if (curent().value == "CONTINUE") {
            result = get_continue_loop_node(FilePath);
        }

    // If the curent type is pure value
    } else if (
        check(TokenType::STRING) ||
        check(TokenType::INTEGER) ||
        check(TokenType::FLOAT) ||
        check(TokenType::BOOLEAN)
    ) {
        result = get_value_node(FilePath);

    // Get between parens
    } else if (check(TokenType::LEFT_PAREN)) {

        // Consume `(`
        auto left_paren_r = consume(TokenType::LEFT_PAREN);

        // Get expression inside parens
        auto node_r = get_expretion(FilePath);

        if (!node_r.success) {
            ReturnResult<Node> re = {
                node_r.Message,
                false,
                nullptr
            };
            re.error_info = node_r.error_info;
            return re;
        }

        // Consume `)`
        auto is_paren_closed__ = consume(TokenType::RIGHT_PAREN);

        if (!is_paren_closed__.success) {
            ReturnResult<Node> re = {
                is_paren_closed__.Message,
                false,
                nullptr
            };
            re.error_info = is_paren_closed__.error_info;
            return re;
        }

        result = std::move(node_r);

    }

    // Stop if parsing the primary node failed
    if (!result.success) {
        return result;
    }

    return result;
}

// ==================================================================
// Expression entry point: OR
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_expretion(const std::string& FilePath) {
    return get_logical_or(FilePath);
}

// ==================================================================
// Logical OR
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_logical_or(const std::string& FilePath) {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_logical_and(FilePath);

    if (!first.success)
        return first;

    if (!check(TokenType::LOGICAL_OR))
        return first;

    parts.push_back({
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    });

    while (check(TokenType::LOGICAL_OR)) {
        auto op = consume(TokenType::LOGICAL_OR);

        if (!op.success)
            return propagate_error(op);

        auto operand_token = curent();
        auto operand = get_logical_and(FilePath);

        if (!operand.success)
            return operand;

        parts.push_back({
            TokenType::LOGICAL_OR,
            operand_token,
            std::move(operand.value)
        });
    }

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<LogicOpsNode>(FilePath, END_token, parts)
    };
}

// ==================================================================
// Logical AND
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_logical_and(const std::string& FilePath) {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_equality(FilePath);

    if (!first.success)
        return first;

    if (!check(TokenType::LOGICAL_AND))
        return first;

    parts.push_back({
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    });

    while (check(TokenType::LOGICAL_AND)) {
        auto op = consume(TokenType::LOGICAL_AND);

        if (!op.success)
            return propagate_error(op);

        auto operand_token = curent();
        auto operand = get_equality(FilePath);

        if (!operand.success)
            return operand;

        parts.push_back({
            TokenType::LOGICAL_AND,
            operand_token,
            std::move(operand.value)
        });
    }

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<LogicOpsNode>(FilePath, END_token, parts)
    };
}

// ==================================================================
// Equality: ==, !=
// Non-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_equality(const std::string& FilePath) {
    auto first_token = curent();
    auto first = get_comparison(FilePath);

    if (!first.success)
        return first;

    if (
        !check(TokenType::EQUAL_EQUAL) &&
        !check(TokenType::NOT_EQUAL)
    ) {
        return first;
    }

    auto op = curent();

    auto consumed = consume(op.Type);

    if (!consumed.success)
        return propagate_error(consumed);

    auto second_token = curent();
    auto second = get_comparison(FilePath);

    if (!second.success)
        return second;

    // Reject chained equality comparisons: a == b == c
    if (
        check(TokenType::EQUAL_EQUAL) ||
        check(TokenType::NOT_EQUAL)
    ) {
        auto error = Errors::SyntaxError(
            curent().value,
            curent().line,
            curent().column,
            curent().value.size()
        );

        ReturnResult<Parser::Node> result = {
            error.msg,
            false,
            nullptr
        };

        result.error_info = error.info;
        return result;
    }

    OperationPart left = {
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    };

    OperationPart right = {
        TokenType::UNKNOWN,
        second_token,
        std::move(second.value)
    };

    auto comparison_type = ComparitonSymbols_ToOps.at(op.Type);

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<CompOpsNode>(FilePath, END_token, left,
            right,
            comparison_type
        )
    };
}

// ==================================================================
// Relational comparisons: <, >, <=, >=
// Non-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_comparison(const std::string& FilePath) {
    auto first_token = curent();
    auto first = get_additive(FilePath);

    if (!first.success)
        return first;

    const auto is_relational = [this, &FilePath]() {
        return
            check(TokenType::LESS_THAN) ||
            check(TokenType::GREATER_THAN) ||
            check(TokenType::LESS_EQUAL) ||
            check(TokenType::GREATER_EQUAL);
    };

    if (!is_relational())
        return first;

    auto op = curent();

    auto consumed = consume(op.Type);

    if (!consumed.success)
        return propagate_error(consumed);

    auto second_token = curent();
    auto second = get_additive(FilePath);

    if (!second.success)
        return second;

    // Reject chained relational comparisons: a < b < c
    if (is_relational()) {
        auto error = Errors::SyntaxError(
            curent().value,
            curent().line,
            curent().column,
            curent().value.size()
        );

        ReturnResult<Parser::Node> result = {
            error.msg,
            false,
            nullptr
        };

        result.error_info = error.info;
        return result;
    }

    OperationPart left = {
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    };

    OperationPart right = {
        TokenType::UNKNOWN,
        second_token,
        std::move(second.value)
    };

    auto comparison_type = ComparitonSymbols_ToOps.at(op.Type);

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<CompOpsNode>(FilePath, END_token, left,
            right,
            comparison_type
        )
    };
}

// ==================================================================
// Addition and subtraction: +, -
// Left-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_additive(const std::string& FilePath) {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_multiplicative(FilePath);

    if (!first.success)
        return first;

    if (
        !check(TokenType::PLUS) &&
        !check(TokenType::MINUS)
    ) {
        return first;
    }

    parts.push_back({
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    });

    while (
        check(TokenType::PLUS) ||
        check(TokenType::MINUS)
    ) {
        auto op = curent();

        auto consumed = consume(op.Type);

        if (!consumed.success)
            return propagate_error(consumed);

        auto operand_token = curent();
        auto operand = get_multiplicative(FilePath);

        if (!operand.success)
            return operand;

        parts.push_back({
            op.Type,
            operand_token,
            std::move(operand.value)
        });
    }

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<BinOpsNode>(FilePath, END_token, parts)
    };
}

// ==================================================================
// Multiplication, division, integer division, modulo
// Left-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_multiplicative(const std::string& FilePath) {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_unary(FilePath);

    if (!first.success)
        return first;

    const auto is_multiplicative = [this, &FilePath]() {
        return
            check(TokenType::STAR) ||
            check(TokenType::SLASH) ||
            check(TokenType::DIV) ||
            check(TokenType::MOD);
    };

    if (!is_multiplicative())
        return first;

    parts.push_back({
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    });

    while (is_multiplicative()) {
        auto op = curent();

        auto consumed = consume(op.Type);

        if (!consumed.success)
            return propagate_error(consumed);

        auto operand_token = curent();
        auto operand = get_unary(FilePath);

        if (!operand.success)
            return operand;

        parts.push_back({
            op.Type,
            operand_token,
            std::move(operand.value)
        });
    }

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<BinOpsNode>(FilePath, END_token, parts)
    };
}

// ==================================================================
// Unary operations: +x, -x, !x, NOT x
// Right-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_unary(const std::string& FilePath) {
    const bool is_plus = check(TokenType::PLUS);
    const bool is_minus = check(TokenType::MINUS);
    const bool is_not_symbol = check(TokenType::EXCLAMATION);

    const bool is_not_keyword =
        curent().Type == TokenType::KEY_WORD &&
        curent().value == "NOT";

    // No unary operator: parse exponentiation
    if (
        !is_plus &&
        !is_minus &&
        !is_not_symbol &&
        !is_not_keyword
    ) {
        return get_power(FilePath);
    }

    // Save the operator before consuming it
    auto op_token = curent();

    auto consumed = consume(op_token.Type);

    if (!consumed.success)
        return propagate_error(consumed);

    // Save the beginning of the operand for error locations
    auto operand_token = curent();

    // Recursion makes unary operators right-associative
    auto operand = get_unary(FilePath);

    if (!operand.success)
        return operand;

    if (is_not_symbol || is_not_keyword) {
        const Token END_token = tokens_list_[pos - 1];
    return {
            "",
            true,
            std::make_unique<LogicalNotNode>(FilePath, END_token, op_token,
                operand_token,
                operand.value
            )
        };
    }

    const auto sign_type = is_minus
        ? SignOpsTypes::NEGATIVE
        : SignOpsTypes::POSITIVE;

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<SignOpNode>(FilePath, END_token, sign_type,
            op_token,
            operand_token,
            operand.value
        )
    };
}

// ==================================================================
// Exponentiation: **
// Right-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_power(const std::string& FilePath) {
    auto first_token = curent();
    auto first = get_primary(FilePath);

    if (!first.success)
        return first;

    if (!check(TokenType::POWER))
        return first;

    auto op_token = curent();

    auto consumed = consume(TokenType::POWER);

    if (!consumed.success)
        return propagate_error(consumed);

    // Parse the exponent as unary:
    // 2 ** 3 ** 2 => 2 ** (3 ** 2)
    // 2 ** -2     => 2 ** (-2)
    auto second_token = curent();
    auto second = get_unary(FilePath);

    if (!second.success)
        return second;

    OperationPart left = {
        TokenType::UNKNOWN,
        first_token,
        std::move(first.value)
    };

    OperationPart right = {
        TokenType::UNKNOWN,
        second_token,
        std::move(second.value)
    };

    const Token END_token = tokens_list_[pos - 1];
    return {
        "",
        true,
        std::make_unique<PowerNode>(FilePath, END_token, op_token,
            left,
            right
        )
    };
}

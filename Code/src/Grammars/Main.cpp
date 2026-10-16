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
Parser::get_primary() {

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
            result = get_functioncall_node();

        // Set or reset variable node
        } else if (curent().value == "SET" || curent().value == "SET_CONST") {
            result = get_set_variable_node(curent().value == "SET_CONST");

        // Get variable's value node
        } else if (curent().value == "GET") {
            result = get_get_variable_node();

        // Get user function
        } else if (curent().value == "FUNCTION") {
            result = get_user_function_node();

        // Get return node
        } else if (curent().value == "RETURN") {
            result = get_return_noode();

        // Get If statement node
        } else if (curent().value == "IF") {
            result = get_if_statment_node();

        // Get the while loop node
        } else if (curent().value == "WHILE") {
            result = get_while_loop_node();

        // Get the for loop node
        } else if (curent().value == "FOR") {
            result = get_for_loop_node();

        // Get the break a loop node
        } else if (curent().value == "BREAK") {
            result = get_break_loop_node();

        // Get the continue a loop node
        } else if (curent().value == "CONTINUE") {
            result = get_continue_loop_node();
        }

    // If the curent type is pure value
    } else if (
        check(TokenType::STRING) ||
        check(TokenType::INTEGER) ||
        check(TokenType::FLOAT) ||
        check(TokenType::BOOLEAN)
    ) {
        result = get_value_node();

    // Get between parens
    } else if (check(TokenType::LEFT_PAREN)) {

        // Consume `(`
        auto left_paren_r = consume(TokenType::LEFT_PAREN);

        // Get expression inside parens
        auto node_r = get_expretion();

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
Parser::get_expretion() {
    return get_logical_or();
}

// ==================================================================
// Logical OR
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_logical_or() {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_logical_and();

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
        auto operand = get_logical_and();

        if (!operand.success)
            return operand;

        parts.push_back({
            TokenType::LOGICAL_OR,
            operand_token,
            std::move(operand.value)
        });
    }

    return {
        "",
        true,
        std::make_unique<LogicOpsNode>(parts)
    };
}

// ==================================================================
// Logical AND
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_logical_and() {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_equality();

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
        auto operand = get_equality();

        if (!operand.success)
            return operand;

        parts.push_back({
            TokenType::LOGICAL_AND,
            operand_token,
            std::move(operand.value)
        });
    }

    return {
        "",
        true,
        std::make_unique<LogicOpsNode>(parts)
    };
}

// ==================================================================
// Equality: ==, !=
// Non-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_equality() {
    auto first_token = curent();
    auto first = get_comparison();

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
    auto second = get_comparison();

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

    return {
        "",
        true,
        std::make_unique<CompOpsNode>(
            left,
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
Parser::get_comparison() {
    auto first_token = curent();
    auto first = get_additive();

    if (!first.success)
        return first;

    const auto is_relational = [this]() {
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
    auto second = get_additive();

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

    return {
        "",
        true,
        std::make_unique<CompOpsNode>(
            left,
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
Parser::get_additive() {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_multiplicative();

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
        auto operand = get_multiplicative();

        if (!operand.success)
            return operand;

        parts.push_back({
            op.Type,
            operand_token,
            std::move(operand.value)
        });
    }

    return {
        "",
        true,
        std::make_unique<BinOpsNode>(parts)
    };
}

// ==================================================================
// Multiplication, division, integer division, modulo
// Left-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_multiplicative() {
    OperationPartsList parts;

    auto first_token = curent();
    auto first = get_unary();

    if (!first.success)
        return first;

    const auto is_multiplicative = [this]() {
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
        auto operand = get_unary();

        if (!operand.success)
            return operand;

        parts.push_back({
            op.Type,
            operand_token,
            std::move(operand.value)
        });
    }

    return {
        "",
        true,
        std::make_unique<BinOpsNode>(parts)
    };
}

// ==================================================================
// Unary operations: +x, -x, !x, NOT x
// Right-associative
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_unary() {
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
        return get_power();
    }

    // Save the operator before consuming it
    auto op_token = curent();

    auto consumed = consume(op_token.Type);

    if (!consumed.success)
        return propagate_error(consumed);

    // Save the beginning of the operand for error locations
    auto operand_token = curent();

    // Recursion makes unary operators right-associative
    auto operand = get_unary();

    if (!operand.success)
        return operand;

    if (is_not_symbol || is_not_keyword) {
        return {
            "",
            true,
            std::make_unique<LogicalNotNode>(
                op_token,
                operand_token,
                operand.value
            )
        };
    }

    const auto sign_type = is_minus
        ? SignOpsTypes::NEGATIVE
        : SignOpsTypes::POSITIVE;

    return {
        "",
        true,
        std::make_unique<SignOpNode>(
            sign_type,
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
Parser::get_power() {
    auto first_token = curent();
    auto first = get_primary();

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
    auto second = get_unary();

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

    return {
        "",
        true,
        std::make_unique<PowerNode>(
            op_token,
            left,
            right
        )
    };
}
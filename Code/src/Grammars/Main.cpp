// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "Errors.hpp"

// Nodes
#include "ASTNodes/BinaryOpsNode.hpp"


// ==================================================================
// Global function of primary nodes
// ==================================================================

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

        if (!left_paren_r.success) {
            ReturnResult<Node> re = {
                left_paren_r.Message,
                false,
                nullptr
            };
            re.error_info = left_paren_r.error_info;
            return re;
        }

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

    // Get negative or positive number
    } else if (
        (check(TokenType::MINUS) || check(TokenType::PLUS)) &&
        (peek(1).Type == TokenType::INTEGER || peek(1).Type == TokenType::FLOAT)
    ) {

        // Consume the sign
        auto op = curent().Type == TokenType::MINUS
            ? consume(TokenType::MINUS)
            : consume(TokenType::PLUS);

        if (!op.success) {
            ReturnResult<Node> re = {
                op.Message,
                false,
                nullptr
            };
            re.error_info = op.error_info;
            return re;
        }

        // Add the sign to the number
        curent_token_.value = op.value.value + curent().value;

        result = get_value_node();

    } else {
        return result;
    }

    // Stop if parsing the primary node failed
    if (!result.success) {
        return result;
    }

    // Comparison operation expressions (==, !=, ...) =============================

    if (
        check(TokenType::EQUAL_EQUAL) ||
        check(TokenType::NOT_EQUAL) ||
        check(TokenType::LESS_THAN) ||
        check(TokenType::GREATER_THAN) ||
        check(TokenType::LESS_EQUAL) ||
        check(TokenType::GREATER_EQUAL)
    ) {

        // Move result to node_r
        auto node_r = std::move(result);

        // Get operation token
        auto op = curent();

        auto consume_op_r = consume(op.Type);

        if (!consume_op_r.success) {
            ReturnResult<Node> re = {
                consume_op_r.Message,
                false,
                nullptr
            };
            re.error_info = consume_op_r.error_info;
            return re;
        }

        // Get operation type
        ComparitonOpsTypes opT;
        opT = ComparitonSymbols_ToOps[op.Type];

        // Get the second value
        auto second_part_T = curent();
        auto scond_part_r = get_primary();

        if (!scond_part_r.success) {
            ReturnResult<Node> re = {
                scond_part_r.Message,
                false,
                nullptr
            };
            re.error_info = scond_part_r.error_info;
            return re;
        }

        // First part
        OperationPart fpart = {
            TokenType::UNKNOWN,
            cu,
            std::move(node_r.value)
        };

        // Second part
        OperationPart spart = {
            TokenType::UNKNOWN,
            second_part_T,
            std::move(scond_part_r.value)
        };

        auto ptr = std::make_unique<CompOpsNode>(
            fpart,
            spart,
            opT
        );

        result = {"", true, std::move(ptr)};
    }

    return result;
}

// ==================================================================
// The main function of getting nodes, starts with + and -
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_expretion() {

    OperationPartsList parts; // Define parts list

    auto cu = curent(); // Get token of first part
    auto node_r = get_term(); // Get the first part

    if (!node_r.success) {
        ReturnResult<Node> re = {
            node_r.Message,
            false,
            nullptr
        };
        re.error_info = node_r.error_info;
        return re;
    }

    // Binary operation expressions (+, -) =============================

    // If token after first part is plus (+) or minus (-)
    if (check(TokenType::PLUS) || check(TokenType::MINUS)) {

        // Push the first node in parts list
        parts.push_back({
            TokenType::UNKNOWN,
            cu,
            std::move(node_r.value)
        });
    }

    // While the curent token is + or -
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {

        // Consume operation symbol
        auto op = check(TokenType::PLUS)
            ? consume(TokenType::PLUS)
            : consume(TokenType::MINUS);

        if (!op.success) {
            ReturnResult<Node> re = {
                op.Message,
                false,
                nullptr
            };
            re.error_info = op.error_info;
            return re;
        }

        auto pcu = curent(); // Get the first token of this part
        auto pnode_r = get_term(); // Get this part

        if (!pnode_r.success) {
            ReturnResult<Node> re = {
                pnode_r.Message,
                false,
                nullptr
            };
            re.error_info = pnode_r.error_info;
            return re;
        }

        // Push this part in parts list
        parts.push_back({
            op.value.Type,
            pcu,
            std::move(pnode_r.value)
        });
    }

    if (!parts.empty()) {
        return {
            "",
            true,
            std::make_unique<BinOpsNode>(
                parts
            )
        };
    }

    // Logical operation expressions (&&, ||) =============================

    // If token after first part is AND (&&) or OR (||)
    if (check(TokenType::LOGICAL_AND) || check(TokenType::LOGICAL_OR)) {

        // Push the first node in parts list
        parts.push_back({
            TokenType::UNKNOWN,
            cu,
            std::move(node_r.value)
        });
    }

    // While the curent token is && or ||
    while (check(TokenType::LOGICAL_AND) || check(TokenType::LOGICAL_OR)) {

        // Consume operation symbol
        auto op = check(TokenType::LOGICAL_AND)
            ? consume(TokenType::LOGICAL_AND)
            : consume(TokenType::LOGICAL_OR);

        if (!op.success) {
            ReturnResult<Node> re = {
                op.Message,
                false,
                nullptr
            };
            re.error_info = op.error_info;
            return re;
        }

        auto pcu = curent(); // Get the first token of this part
        auto pnode_r = get_term(); // Get this part

        if (!pnode_r.success) {
            ReturnResult<Node> re = {
                pnode_r.Message,
                false,
                nullptr
            };
            re.error_info = pnode_r.error_info;
            return re;
        }

        // Push this part in parts list
        parts.push_back({
            op.value.Type,
            pcu,
            std::move(pnode_r.value)
        });
    }

    if (!parts.empty()) {
        return {
            "",
            true,
            std::make_unique<LogicOpsNode>(
                parts
            )
        };
    }

    return node_r;
}

// ==================================================================
// The function that gets * and / if they are
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_term() {

    OperationPartsList parts; // Define parts list

    auto cu = curent(); // Get token of first part
    auto node_r = get_primary(); // Get the first part

    if (!node_r.success) {
        ReturnResult<Node> re = {
            node_r.Message,
            false,
            nullptr
        };
        re.error_info = node_r.error_info;
        return re;
    }

    // If token after first part is multiplication or division
    if (check(TokenType::STAR) || check(TokenType::SLASH)) {

        // Push the first node in parts list
        parts.push_back({
            TokenType::UNKNOWN,
            cu,
            std::move(node_r.value)
        });
    }

    // While the curent token is * or /
    while (check(TokenType::STAR) || check(TokenType::SLASH)) {

        // Consume operation symbol
        auto op = check(TokenType::STAR)
            ? consume(TokenType::STAR)
            : consume(TokenType::SLASH);

        if (!op.success) {
            ReturnResult<Node> re = {
                op.Message,
                false,
                nullptr
            };
            re.error_info = op.error_info;
            return re;
        }

        auto pcu = curent(); // Get the first token of this part
        auto pnode_r = get_primary(); // Get this part

        if (!pnode_r.success) {
            ReturnResult<Node> re = {
                pnode_r.Message,
                false,
                nullptr
            };
            re.error_info = pnode_r.error_info;
            return re;
        }

        // Push this part in parts list
        parts.push_back({
            op.value.Type,
            pcu,
            std::move(pnode_r.value)
        });
    }

    if (!parts.empty()) {
        return {
            "",
            true,
            std::make_unique<BinOpsNode>(
                parts
            )
        };
    }

    return node_r;
}
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

    // Set result var
    ReturnResult<Parser::Node> result = {
        Errors::SyntaxError(
            curent().value,
            curent().line,
            curent().column
        ).msg,
        false,
        nullptr
    };

    // set match result var
    ReturnResult<bool> mt;

    // If the curent token is key word  (like CALL and SET)
    if (curent().Type == TokenType::KEY_WORD) {

        // Function call node
        if (curent().value == "CALL") {
            result = get_functioncall_node();

        // Set or reset variable node
        } else if (curent().value == "SET" || curent().value == "SET_CONST") {
            result = get_set_variable_node(curent().value == "SET_CONST");

        // Get variable's value node
        } else if (curent().value == "GET"){
            result = get_get_variable_node();
        
        // Get user function
        } else if (curent().value == "FUNCTION") {
            result = get_user_function_node(); 
        
        // Get return node 
        } else if (curent().value == "RETURN") {
            result = get_return_noode();

        // Get If statment node
        } else if (curent().value == "IF") {
            result = get_if_statment_node();
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
        consume(TokenType::LEFT_PAREN);
        auto node_r = get_expretion();
        auto is_paren_closed__ = consume(TokenType::RIGHT_PAREN);
        if (!is_paren_closed__.success) return {is_paren_closed__.Message,false,nullptr};
        result = std::move(node_r);

    // Get negative number
    } else if (
        (check(TokenType::MINUS) || check(TokenType::PLUS)) && 
        (peek(1).Type == TokenType::INTEGER || peek(1).Type == TokenType::FLOAT)
    ) {
        auto op = curent().Type == TokenType::MINUS ? consume(TokenType::MINUS) : consume(TokenType::PLUS);
        curent_token_.value = op.value.value + curent().value;
        result = get_value_node();
        
    } else {
        return result;
    }



    // Comparition operation expressions (==,!= ...) =============================3
    if (   
        check(TokenType::EQUAL_EQUAL) || check(TokenType::NOT_EQUAL) || 
        check(TokenType::LESS_THAN) || check(TokenType::GREATER_THAN) ||
        check(TokenType::LESS_EQUAL) || check(TokenType::GREATER_EQUAL)
    ){
        // move result to node_r
        auto node_r = std::move(result);

        // Get operation token
        auto op = curent();
        consume(op.Type);

        // Get Operation Type
        ComparitonOpsTypes opT;
        opT = ComparitonSymbols_ToOps[op.Type];

        // Get the secound value
        auto second_part_T = curent();
        auto scond_part_r = get_primary();
        if (!scond_part_r.success) return scond_part_r; // Error if second part returns error

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
// The mian function of getting nodes, starts with + and -
// ==================================================================

ReturnResult<Parser::Node> 
Parser::get_expretion() {

    OperationPartsList parts; // Define parts list

    auto cu = curent(); // Get token of first part
    auto node_r = get_term(); // Get the first part
    if (!node_r.success) return node_r; // Error if first part returns error

    // Binary operation expressions (+,-) =============================

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
        auto op = check(TokenType::PLUS)? consume(TokenType::PLUS) : consume(TokenType::MINUS);

        auto pcu = curent(); // get the first token of this part
        auto pnode_r = get_term(); // Get this part
        if (!pnode_r.success) return {pnode_r.Message,false,nullptr}; // Error if this part returns error

        // Push this part in parts list
        parts.push_back({
            op.value.Type,
            pcu,
            std::move(pnode_r.value)
        });

    }

    if (!parts.empty()) {
        return {
            "",true,
            std::make_unique<BinOpsNode>(
                parts
            )
        };
    }

    // Logical operation expressions (&&,||) =============================

    // If token after first part is AND (&) or OR (||)
    if (check(TokenType::LOGICAL_AND) || check(TokenType::LOGICAL_OR)) {
        // Push the first node in parts list
        parts.push_back({
            TokenType::UNKNOWN,
            cu,
            std::move(node_r.value)
        });
    } ;

    // While the curent token is && or ||
    while (check(TokenType::LOGICAL_AND) || check(TokenType::LOGICAL_OR)) {

        // Consume operation symbol
        auto op = check(TokenType::LOGICAL_AND)? consume(TokenType::LOGICAL_AND) : consume(TokenType::LOGICAL_OR);

        auto pcu = curent(); // get the first token of this part
        auto pnode_r = get_term(); // Get this part
        if (!pnode_r.success) return {pnode_r.Message,false,nullptr}; // Error if this part returns error

        // Push this part in parts list
        parts.push_back({
            op.value.Type,
            pcu,
            std::move(pnode_r.value)
        });

    }

    if (!parts.empty()) {
        return {
            "",true,
            std::make_unique<LogicOpsNode>(
                parts
            )
        };
    }

    return node_r;
}

// ==================================================================
// The function that get * and / if they are
// ==================================================================

ReturnResult<Parser::Node> 
Parser::get_term() {

    OperationPartsList parts; // Define parts list

    auto cu = curent(); // Get token of first part
    auto node_r = get_primary(); // Get the first part
    if (!node_r.success) return {node_r.Message,false,nullptr}; // Error if first part returns error

    // If token after first part is multiple or divide
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
        auto op = check(TokenType::STAR)? consume(TokenType::STAR) : consume(TokenType::SLASH);

        auto pcu = curent(); // get the first token of this part
        auto pnode_r = get_primary(); // Get this part
        if (!pnode_r.success) return {pnode_r.Message,false,nullptr}; // Error if this part returns error

        // Push this part in parts list
        parts.push_back({
            op.value.Type,
            pcu,
            std::move(pnode_r.value)
        });

    }
    
    if (!parts.empty()) {
        return {
            "",true,
            std::make_unique<BinOpsNode>(
                parts
            )
        };
    } 

    return node_r;
}
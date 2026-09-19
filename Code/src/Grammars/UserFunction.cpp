// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "Errors.hpp"

// Nodes
#include "ASTNodes/UserFunctionNode.hpp"

// ==================================================================
// Parse `FUNCTION`
// ==================================================================

// Get the node of user's function that put the function in symbol table
ReturnResult<Parser::Node> 
Parser::get_user_function_node() {
    ReturnResult<Token> consume_result; // The global cunsum return result object

    std::vector<std::unique_ptr<UserFunctionMethodNode>> methods_list; // methods list of function
    UserFunctionNode::NodesListT statements; // All nodes of code that is in function node
    
    // Jump on 'FUNCTION'
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get function namme
    auto func_name_TR = consume(TokenType::IDENTIFIER);
    if (!func_name_TR.success) return {func_name_TR.Message,false,nullptr};

    // Jump on '('
    consume_result = consume(TokenType::LEFT_PAREN); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get methods
    while (!check(TokenType::RIGHT_PAREN) && !isAsEnd()) {

        // Get method name
        auto method_name_TR = consume(TokenType::IDENTIFIER);
        if (!method_name_TR.success) return {method_name_TR.Message,false,nullptr};

        // Jump on `<`
        consume_result = consume(TokenType::LESS_THAN); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get expected type of method
        auto method_type_TR = consume(TokenType::TYPE);
        if (!method_type_TR.success) return {method_type_TR.Message,false,nullptr};

        // Jump on `>`
        consume_result = consume(TokenType::GREATER_THAN); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get the default value node
        Node default_value_node = nullptr;
        Token default_value_ftoken = {TokenType::UNKNOWN,""};
        if (check(TokenType::EQUAL)) {

            // Jump on '='
            consume_result = consume(TokenType::EQUAL); 
            if (!consume_result.success) return {consume_result.Message,false,nullptr};

            // Copy the first token of value node
            default_value_ftoken = curent();

            // Get the default value node
            auto defualt_value_r = get_expretion();
            if (!defualt_value_r.success) return {defualt_value_r.Message,false,nullptr};

            // Put default value node in default value node
            default_value_node = std::move(defualt_value_r.value);
        }

        // Put the method in methods list
        methods_list.push_back(
            std::make_unique<UserFunctionMethodNode>(
                method_name_TR.value,
                method_type_TR.value,
                default_value_ftoken,
                std::move(default_value_node)
            )
        );

    }

    // Jump on ')'
    consume_result = consume(TokenType::RIGHT_PAREN); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Jump on '-'
    consume_result = consume(TokenType::MINUS); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Jump on '>'
    consume_result = consume(TokenType::GREATER_THAN); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get function return type
    auto func_return_type_TR = consume(TokenType::TYPE);
    if (!func_return_type_TR.success) return {func_return_type_TR.Message,false,nullptr};

    // Get all statments
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {

        // Get statment node
        auto stm_r = get_expretion();
        if (!stm_r.success) return {stm_r.Message,false,nullptr};

        // Put the statment in the statments list
        statements.push_back(std::move(stm_r.value));
    }

    // Jump on 'END'
    consume_result = consume(TokenType::END_BLOCK); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Create the user function node and return it
    return {
        "",true,
        std::make_unique<UserFunctionNode>(
            func_name_TR.value,
            func_return_type_TR.value,
            std::move(methods_list),
            std::move(statements),
            false
        )
    };

}

// ==================================================================
// Parse `RETURN`
// ==================================================================

// Get the node of returning value in function
ReturnResult<Parser::Node> 
Parser::get_return_noode() {
    ReturnResult<Token> consume_result; // The global cunsum return result object

    // Jump on 'RETURN'
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get return value node
    std::unique_ptr<ASTNode> value_node = nullptr;

    // if returns void
    if (curent().value == "void") {

    // Get the value node if it is not void
    } else {
        auto r = get_expretion();
        if (!r.success) return r;
        value_node = std::move(r.value);
    }

    return {
        "",true,
        std::make_unique<UserFunctionReturnNode>(std::move(value_node))
    };

}
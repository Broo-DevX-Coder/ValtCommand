// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"

// Nodes
#include "ASTNodes/FunctionCallNode.hpp"

// ==================================================================
// Parse `CALL`
// ==================================================================

// Call a function with its arguments
ReturnResult<Parser::Node>
Parser::get_functioncall_node() {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `CALL`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success){ 
        ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
        re.error_info = consume_result.error_info;
        return re;
    }

    // Get function name
    auto func_name_t = consume(TokenType::IDENTIFIER);
    if (!func_name_t.success) { 
        ReturnResult<Node> re =  {func_name_t.Message,false,nullptr};
        re.error_info = func_name_t.error_info;
        return re;
    }

    FunctionCallNode::ArgsT arg_list;
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
        
        // Get argument name
        auto arg_name_t = consume(TokenType::IDENTIFIER);
        if (!arg_name_t.success) { 
            ReturnResult<Node> re =  {arg_name_t.Message,false,nullptr};
            re.error_info = arg_name_t.error_info;
            return re;
        }

        // Jump on `<`
        consume_result = consume(TokenType::LESS_THAN); 
        if (!consume_result.success) { 
            ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
            re.error_info = consume_result.error_info;
            return re;
        };

        // Get expected type of arg
        auto arg_type_t = consume(TokenType::TYPE);
        if (!arg_type_t.success) { 
            ReturnResult<Node> re =  {arg_type_t.Message,false,nullptr};
            re.error_info = arg_type_t.error_info;
            return re;
        }

        // Jump on `>`
        consume_result = consume(TokenType::GREATER_THAN); 
        if (!consume_result.success) { 
            ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
            re.error_info = consume_result.error_info;
            return re;
        };

        // Jump on `:`
        consume_result = consume(TokenType::COLON); 
        if (!consume_result.success) { 
            ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
            re.error_info = consume_result.error_info;
            return re;
        };

        // Get arg value node
        auto arg_value = get_expretion();
        if (!arg_value.success) return {arg_value.Message,false,nullptr};

        // Add the argument to args list
        arg_list.push_back(std::make_unique<FunctionCallArgumentNode>(
            arg_name_t.value.value,
            arg_type_t.value,
            std::move(arg_value.value)
        ));
    }

    // Jump on `END`
    consume_result = consume(TokenType::END_BLOCK); 
    if (!consume_result.success) { 
        ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
        re.error_info = consume_result.error_info;
        return re;
    };

    // Create FunctionCallNode and get the value
    return {"",true,std::make_unique<FunctionCallNode>(
        func_name_t.value.value,
        arg_list,
        func_name_t.value
    )};

}
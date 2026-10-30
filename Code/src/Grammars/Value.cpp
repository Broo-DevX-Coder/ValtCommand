// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"

// Nodes
#include "ASTNodes/ValueNode.hpp"


// ==================================================================
// Get the pure value's node
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_value_node(const std::string& FilePath) {
    if (curent().Type == TokenType::STRING) {
        auto value_t = consume(TokenType::STRING);
        const Token END_token = value_t.value;
        return {"",true,std::make_unique<StringValueNode>(FilePath, END_token, value_t.value)};

    // If the curent type is intiger value
    } else if (curent().Type == TokenType::INTEGER) {
        auto value_t = consume(TokenType::INTEGER);
        const Token END_token = value_t.value;
        return {"",true,std::make_unique<NumberValueNode>(FilePath, END_token, value_t.value,"int")};

    // If the curent type is float value
    } else if (curent().Type == TokenType::FLOAT) {
        auto value_t = consume(TokenType::FLOAT);
        const Token END_token = value_t.value;
        return {"",true,std::make_unique<NumberValueNode>(FilePath, END_token, value_t.value,"float")};
    }

    // If the curent type is boolean value
    auto value_t = consume(TokenType::BOOLEAN);
    const Token END_token = value_t.value;
    return {"",true,std::make_unique<BooleanValueNode>(FilePath, END_token, value_t.value)};

}

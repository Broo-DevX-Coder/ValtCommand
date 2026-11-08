// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "detail/parser.hpp"
#include "detail/errors.hpp"

// ==================================================================
// Parses's functions
// ==================================================================

// Constructure
namespace vltc {
Parser::Parser(
    TList& tl_,
    const std::string& FilePath
): tokens_list_(std::move(tl_)) {

    if (tokens_list_.empty()) {
        is_code_ended_ = true;
        return;
    }
    curent_token_ = tokens_list_[0];
}

// Go to the next token
void
Parser::advence() {
    pos++;
    if (pos < tokens_list_.size()) {
        curent_token_ = tokens_list_[pos];
        if (curent_token_.Type == TokenType::END_CODE)
            is_code_ended_ = true;
        //std::cout << curent_token_.value << std::endl << std::flush;

    } else {
        is_code_ended_ = true;
        curent_token_ = tokens_list_[tokens_list_.size()-1];
        curent_token_.value = "(end of code!)";
    }
}

// Get the curent token
const Token&
Parser::curent() {
    return curent_token_;
}

// Get the next <offset> token
const Token&
Parser::peek(size_t offset
) {
    if (pos+offset < tokens_list_.size())
        return tokens_list_[pos+offset];
    return tokens_list_[tokens_list_.size()-1];
}

// Check the curent token
bool
Parser::check(TokenType type) {
    return curent().Type == type;
}

// Check the curent token and move to next
ReturnResult<Token>
Parser::consume(TokenType type) {
    if (curent().Type != type){
        const bool at_end = curent_token_.Type == TokenType::END_CODE || isAsEnd();
        size_t error_line = curent_token_.line;
        size_t error_column = curent_token_.column;
        if (at_end && pos > 0) {
            const Token& previous = tokens_list_[pos - 1];
            error_line = previous.line;
            error_column = previous.column + previous.value.size();
        }
        auto err = curent_token_.Type == TokenType::UNTERMINATED_STRING ||
                   curent_token_.Type == TokenType::UNTERMINATED_COMMENT
            ? Errors::SyntaxError(curent_token_)
            : Errors::SyntaxError(
                at_end ? "(end of code!)" : curent_token_.value,
                error_line,
                error_column,
                at_end ? 1 : curent_token_.value.size(),
                at_end ? "Unexpected end of input while parsing" : ""
            );

        ReturnResult<Token> re = {err.msg, false, curent()};
        re.error_info = err.info;
        return re;
    }

    auto t = curent();
    advence();

    return {"", true, t};
}

// Is the code as end
bool
Parser::isAsEnd() {
    return is_code_ended_;
}

// Get clear module node ==================================================
ReturnResult<Parser::PNode>
Parser::get_module_node(const std::string& FilePath) {

    ModuleNode::StatmentsT s_list;
    ReturnResult<Parser::Node> nt;
    Token FirstT = curent();

    do {
        Token FT = curent();
        nt = get_expretion(FilePath);
        if (!nt.success) {
            ReturnResult<PNode> re = {nt.Message, false, nullptr};
            re.error_info = nt.error_info;
            return re;
        }
        s_list.push_back({std::move(nt.value),FT});
    } while (!isAsEnd());

    // The final consumed source token is the module end token.
    const Token END_token = tokens_list_[pos > 0 ? pos - 1 : 0];

    // make module node and return them
    return {
        "",
        true,
        std::make_unique<ModuleNode>(
            FilePath,
            END_token,
            s_list,
            FirstT
        )
    };
}

} // namespace vltc

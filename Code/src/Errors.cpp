// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <fmt/format.h>

// == Local ==
#include "Errors.hpp"

// ==================================================================
// Namespace
// ==================================================================

// ============== SyntaxError class constructure ==============
Errors::SyntaxError::SyntaxError(
    std::string token, 
    int line, 
    int column, 
    std::string comment
) {
    msg = fmt::format(
        "SyntaxError: expected '{}' at line:{} ,column:{}",
        token, line, column
    );
    if (!comment.empty())
        msg+=fmt::format("\n{}",comment);
}

// ============== TypeError class constructure ==============
Errors::TypeError::TypeError(
    std::string expected, 
    std::string got, 
    int line, 
    int column, 
    std::string comment
) {
    msg = fmt::format(
        "TypeError: expected '{}', got '{}', at line:{} ,column:{}",
        expected, got, line, column
    );
    if (!comment.empty())
        msg+=fmt::format("\n{}",comment);
}

// ============== NameError class constructure ==============
Errors::NameError::NameError(
    std::string name, 
    int line, 
    int column, 
    std::string comment
) {
    msg = fmt::format(
        "NameError: '{}' is not defined at line:{} ,column:{}",
        name, line, column
    );
    if (!comment.empty())
        msg+=fmt::format("\n{}",comment);
}


// ============== NameError class constructure ==============
Errors::ArgumentError::ArgumentError(
    std::string f_name,
    int l_, 
    int c_, 
    std::string com_
):
    function_name(f_name),
    line(l_),
    column(c_),
    comment(com_) {}

// When the user places more arguments than expacted
std::string 
Errors::ArgumentError::too_many_arguments(
    int expected, 
    int placed
) {
    msg = fmt::format(
        "ArgumentError: Function '{}': Too many arguments. Expected {}, but got {} at line:{}, column:{}",
        function_name, expected, placed, line, column
    );
    return msg;
}

// When user places unknown argument
std::string 
Errors::ArgumentError::unexpected_argument(
    std::string arg_name
) {
    msg = fmt::format(
        "ArgumentError: Function '{}': Unexpected argument '{}' at line:{}, column:{}",
        function_name, arg_name, line, column
    );
    return msg;
}

// When user don't places all required arguments
std::string 
Errors::ArgumentError::unplaced_arg(
    std::string arg_name
) {
    msg = fmt::format(
        "ArgumentError: Function '{}': Missing required argument '{}' at line:{}, column:{}",
        function_name, arg_name, line, column
    );
    return msg;
}

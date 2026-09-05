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
        "SyntaxError: unexpected token '{}' at line:{} ,column:{}",
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


// ============== ArgumentError class constructure ==============
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

std::string 
Errors::ArgumentError::unexpected_argument_type(
    std::string arg_name, 
    std::string expected_type, 
    std::string placed_type
) {
    msg = fmt::format(
        "ArgumentError: Function '{}': Unexpected argument '{}' type expected:{}, got:{}  at line:{}, column:{}",
        function_name, arg_name, expected_type, placed_type, line, column
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

// ============== VariableError class constructure ==============

Errors::VariableError::VariableError(
    std::string v_name,
    int l_, 
    int c_, 
    std::string com_
):
    var_name(v_name),
    line(l_),
    column(c_),
    comment(com_) {}

// When recreate variable in same scope
std::string 
Errors::VariableError::reset_var_in_same_scope() {
    msg = fmt::format(
        "VariableError: Variable '{}' already defined in this scope at line:{}, column:{}",
        var_name, line, column
    );
    return msg;
}



// When reset variable value but new value is incompatible type with variable type
std::string 
Errors::VariableError::reset_var_value_by_other_type(
    std::string var_type, 
    std::string got_type
) {
    msg = fmt::format(
        "TypeError: Cannot assign value of type '{}' to variable '{}' of type '{}' at line:{}, column:{}",
        got_type, var_name, var_type, line, column
    );
    return msg;
}

// When user reset a const variable
std::string 
Errors::VariableError::reset_const_var() {
    msg = fmt::format(
        "ImmutableError: Cannot modify constant variable '{}' at line:{}, column:{}",
        var_name, line, column
    );
    return msg;
}

// When user reset variable value but from auther scoupe (inside function or authe module)
std::string 
Errors::VariableError::reset_var_from_auther_scope() {
    msg = fmt::format(
        "ScopeError: Cannot modify variable '{}' declared in another scope at line:{}, column:{}",
        var_name, line, column
    );
    return msg;
}
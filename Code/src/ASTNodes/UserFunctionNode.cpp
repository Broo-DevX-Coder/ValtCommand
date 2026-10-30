// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/UserFunctionNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>
#include <string>
#include <utility>
#include <unordered_map>

// ==================================================================
// Runtime Tracker helper
// ==================================================================

static void emit_event(
    NodesContext& Context,
    DevTools::EventsT type,
    const std::string& message,
    std::size_t line
) {
    if (Context.Debugger) {
        Context.Debugger->emit({
            type,
            message,
            line,
            Context.ParentScope,
            Context.FilePath
        });
    }
}

// ==================================================================
// User Function argument node functions
// ==================================================================

// Constructor
UserFunctionArgumentNode::UserFunctionArgumentNode(const std::string& FilePath,
    const Token& END_token,
    Token& nameT,
    Token& typeT,
    Token& default_value_FT,
    std::unique_ptr<ASTNode> default_value_N,
    bool is_argument_any
):
    name_token(nameT),
    type_token(typeT),
    default_value_node(std::move(default_value_N)),
    default_value_ftoekn(default_value_FT),
    is_argument_any(is_argument_any),
    ASTNode(FilePath, END_token) {}


// Get string for print
std::string
UserFunctionArgumentNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "argument " << name_token.value << ":\n";

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "| type: " << type_token.value << "\n";

    if (default_value_node != nullptr) {
        for (int i = 0; i < level; i++)
            ss << "|  ";

        ss << "| default value:\n";

        ss << default_value_node->get_str(level + 1);
    }

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
UserFunctionArgumentNode::get_json() {
    nlohmann::json result;

    result["type"] = "UserFunctionargument";
    result["name"] = name_token.value;
    result["return_type"] = type_token.value;

    if (default_value_node != nullptr)
        result["default_value"] = default_value_node->get_json();
    else
        result["default_value"] = nullptr;

    return result;
}


// Get type of node
ASTNodesTypes
UserFunctionArgumentNode::NType() {
    return NT__UserFunctionArgumentNode;
}


// Verify argument
ReturnResult<bool>
UserFunctionArgumentNode::accept(
    NodesContext Context
) {
    return_type = type_token.value;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing parameter '" + name_token.value +
            "' of type '" + type_token.value + "'",
        name_token.line
    );

    // Verify default value node
    if (default_value_node != nullptr) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing default value of parameter '" +
                name_token.value + "'",
            default_value_ftoekn.line
        );

        auto result = default_value_node->accept(Context);

        if (!result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Failed to analyze default value of parameter '" +
                    name_token.value + "'",
                default_value_ftoekn.line
            );

            return result;
        }

        if (!are_types_compatible(
                default_value_node->return_type,
                return_type
            )) {
            auto err = Errors::TypeError(
                return_type,
                default_value_node->return_type,
                default_value_ftoekn.line,
                default_value_ftoekn.column,
                default_value_ftoekn.value.size()
            );

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Parameter '" + name_token.value +
                    "' has an incompatible default value type",
                default_value_ftoekn.line
            );

            ReturnResult<bool> re = {
                err.msg,
                false,
                false
            };

            re.error_info = err.info;
            return re;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Parameter '" + name_token.value +
            "' passed verification",
        name_token.line
    );

    return {"", true, true};
}


// Execute node
ReturnResult<Value>
UserFunctionArgumentNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    return_type = type_token.value;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Processing parameter '" + name_token.value + "'",
        name_token.line
    );

    if (default_value_node != nullptr) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Evaluating default value of parameter '" +
                name_token.value + "'",
            default_value_ftoekn.line
        );

        auto vnode_r = default_value_node->exec(Context);

        if (!vnode_r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Failed to evaluate default value of parameter '" +
                    name_token.value + "'",
                default_value_ftoekn.line
            );

            return vnode_r;
        }

        default_value = reconsiliation_int_float(
            type_token.value,
            vnode_r.value
        );

        default_value_node = nullptr;

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Default value of parameter '" +
                name_token.value + "' evaluated successfully",
            default_value_ftoekn.line
        );
    }

    return {"", true, std::monostate{}};
}


// Copy class
ASTNode*
UserFunctionArgumentNode::clone() {
    std::unique_ptr<ASTNode> new_defualt_value_node(
        default_value_node == nullptr
            ? nullptr
            : default_value_node->clone()
    );

    return new UserFunctionArgumentNode(file, END_token, name_token,
        type_token,
        default_value_ftoekn,
        std::move(new_defualt_value_node),
        is_argument_any
    );
}


// ==================================================================
// User Function's return node
// ==================================================================

// Constructor
UserFunctionReturnNode::UserFunctionReturnNode(const std::string& FilePath,
    const Token& END_token,
    Token& t_,
    std::unique_ptr<ASTNode> value_node
):
    VNode(std::move(value_node)),
    ReturnT(t_),
    ASTNode(FilePath, END_token) {}


// Get string to print
std::string
UserFunctionReturnNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Return value:\n";

    if (VNode != nullptr)
        ss << VNode->get_str(level + 1);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
UserFunctionReturnNode::get_json() {
    nlohmann::json result;

    result["type"] = "Return";

    if (VNode != nullptr)
        result["value"] = VNode->get_json();
    else
        result["value"] = nullptr;

    return result;
}


// Type of node
ASTNodesTypes
UserFunctionReturnNode::NType() {
    return NT__UserFunctionReturnNode;
}


// Verify before running
ReturnResult<bool>
UserFunctionReturnNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing RETURN statement",
        ReturnT.line
    );

    if (VNode == nullptr) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "RETURN statement has no value expression",
            ReturnT.line
        );

        return {
            "RETURN statement has no value expression",
            false,
            false,
            ExecState::Return
        };
    }

    auto r = VNode->accept(Context);

    if (!r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Failed to analyze RETURN expression",
            ReturnT.line
        );

        ReturnResult<bool> re = {
            r.Message,
            false,
            false,
            ExecState::Return
        };

        re.error_info = r.error_info;
        return re;
    }

    return_type = VNode->return_type;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "RETURN expression has type '" + return_type + "'",
        ReturnT.line
    );

    ReturnResult<bool> res = {
        "",
        true,
        true,
        ExecState::Return
    };

    res.return_nodes.push_back(this);

    return res;
}


// Execute node and get value
ReturnResult<Value>
UserFunctionReturnNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing RETURN statement",
        ReturnT.line
    );

    ReturnResult<Value> res;

    if (VNode == nullptr) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "RETURN statement has no value expression",
            ReturnT.line
        );

        return {
            "RETURN statement has no value expression",
            false,
            std::monostate{},
            ExecState::Return
        };
    }

    auto vnode_r = VNode->exec(Context);

    if (!vnode_r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Failed to evaluate RETURN expression",
            ReturnT.line
        );

        res = {
            vnode_r.Message,
            false,
            std::monostate{},
            ExecState::Return
        };

        res.return_nodes.push_back(this);
        res.error_info = vnode_r.error_info;

        return res;
    }

    return_type = VNode->return_type;

    // Preserve the original behavior: this node is no longer needed
    // after its return expression has been evaluated.
    VNode = nullptr;

    res = {
        "",
        true,
        std::monostate{},
        ExecState::Return
    };

    res.return_nodes.push_back(this);
    res.return_data = vnode_r.value;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "RETURN completed with type '" + return_type + "'",
        ReturnT.line
    );

    return res;
}


// Copy class
ASTNode*
UserFunctionReturnNode::clone() {
    std::unique_ptr<ASTNode> new_VNode(
        VNode == nullptr ? nullptr : VNode->clone()
    );

    return new UserFunctionReturnNode(file, END_token, ReturnT,
        std::move(new_VNode)
    );
}


// ==================================================================
// User Proxy Function node
// ==================================================================

// Constructor
UserProxyFunctionNode::UserProxyFunctionNode(const std::string& FilePath,
    const Token& END_token,
    NodesListT statements,
    const std::string& type
):
    statements(std::move(statements)),
    ASTNode(FilePath, END_token) {
    return_type = type;
}


// Get string to print
std::string
UserProxyFunctionNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "User Function\n";

    for (auto& statement : statements) {
        if (statement != nullptr)
            ss << statement->get_str(level + 1);
    }

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
UserProxyFunctionNode::get_json() {
    nlohmann::json result;

    result["type"] = "UserFunctionProxy";
    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement != nullptr)
            result["statements"].push_back(statement->get_json());
    }

    return result;
}


// Get type
ASTNodesTypes
UserProxyFunctionNode::NType() {
    return NT__UserProxyFunctionNode;
}


// Verify before running
ReturnResult<bool>
UserProxyFunctionNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing user function body",
        0
    );

    for (auto& stm : statements) {
        if (stm == nullptr)
            continue;

        auto r = stm->accept(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "User function body analysis failed",
                0
            );

            return r;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "User function body passed analysis",
        0
    );

    return {"", true, true};
}


// Execute node
ReturnResult<Value>
UserProxyFunctionNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing user function body",
        0
    );

    for (auto& statement : statements) {
        if (statement == nullptr)
            continue;

        auto r = statement->exec(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "User function body execution failed",
                0
            );

            return r;
        }

        if (r.state == ExecState::Return) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "User function returned a value",
                0
            );

            return {
                "",
                true,
                reconsiliation_int_float(
                    return_type,
                    r.return_data
                )
            };
        }

        statement = nullptr;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "User function body completed without RETURN",
        0
    );

    return {"", true, std::monostate{}};
}


// Copy class
ASTNode*
UserProxyFunctionNode::clone() {
    NodesListT new_statments;

    for (auto& s : statements) {
        if (s == nullptr) {
            new_statments.push_back(nullptr);
            continue;
        }

        std::unique_ptr<ASTNode> stm(s->clone());
        new_statments.push_back(std::move(stm));
    }

    return new UserProxyFunctionNode(file, END_token, std::move(new_statments),
        return_type
    );
}


// ==================================================================
// User Function node
// ==================================================================

// Constructor
UserFunctionNode::UserFunctionNode(const std::string& FilePath,
    const Token& END_token,
    Token& nameT,
    Token& typeT,
    argumentsList arguments_list,
    NodesListT statements,
    bool is_any
):
    name_token(nameT),
    return_type_token(typeT),
    arguments_list(std::move(arguments_list)),
    statements(std::move(statements)),
    is_function_any(is_any),
    ASTNode(FilePath, END_token) {}


// Get string to print
std::string
UserFunctionNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "User Function " << name_token.value << ":\n";

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "|  type: " << return_type_token.value << "\n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "arguments:\n";

    for (auto& argument : arguments_list) {
        if (argument != nullptr)
            ss << argument->get_str(level + 2);
    }

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Statements:\n";

    for (auto& statement : statements) {
        if (statement != nullptr)
            ss << statement->get_str(level + 2);
    }

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
UserFunctionNode::get_json() {
    nlohmann::json result;

    result["type"] = "UserFunction";
    result["name"] = name_token.value;
    result["return_type"] = return_type_token.value;

    result["arguments"] = nlohmann::json::array();

    for (const auto& argument : arguments_list) {
        if (argument != nullptr)
            result["arguments"].push_back(argument->get_json());
    }

    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement != nullptr)
            result["statements"].push_back(statement->get_json());
    }

    return result;
}

// Get node type
ASTNodesTypes
UserFunctionNode::NType() {
    return NT__UserFunctionNode;
}


// Verify node
ReturnResult<bool>
UserFunctionNode::accept(
    NodesContext Context
) {
    return_type = "void";

    bool is_there_return = false;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing function declaration '" + name_token.value +
            "' with return type '" + return_type_token.value + "'",
        name_token.line
    );

    // Check whether the function already exists in the same scope
    auto search_result =
        Context.ParentScope->search_function(name_token);

    if (search_result.success) {
        if (search_result.value->scope_id ==
            Context.ParentScope->get_id()) {

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Function declaration failed: '" +
                    name_token.value +
                    "' is already defined in this scope",
                name_token.line
            );

            auto err = Errors::UserFunctionError(
                name_token.value,
                name_token.line,
                name_token.column,
                name_token.value.size()
            );

            ReturnResult<bool> re = {
                err.function_already_defined(),
                false,
                false
            };

            re.error_info = err.info;
            return re;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Creating temporary scope for function '" +
            name_token.value + "'",
        name_token.line
    );

    // Create a new scope for verification
    auto func_scoupe =
        std::make_unique<Scopes::Scope>(Context.ParentScope);

    // Create arguments list to add function in Scope
    std::unordered_map<
        std::string,
        Scopes::SymbolTableTypes::Argument
    > MList;

    // Verify all arguments
    for (auto& argument : arguments_list) {
        if (argument == nullptr)
            continue;

        auto r = argument->accept(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Function parameter verification failed in '" +
                    name_token.value + "'",
                argument->name_token.line
            );

            return r;
        }

        // Put the argument in scope
        Value arg_v = std::monostate();

        func_scoupe->add_var(
            argument->name_token.value,
            argument->type_token.value,
            arg_v,
            true
        );

        // Put the argument in arguments list
        MList[argument->name_token.value] = {
            argument->type_token.value,
            true,
            argument->default_value_node == nullptr,
            argument->is_argument_any,
            std::monostate{}
        };

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Registered parameter '" + argument->name_token.value +
                "' in function scope",
            argument->name_token.line
        );
    }


    // Verify all statements
    for (auto& statement : statements) {
        if (statement == nullptr)
            continue;

        auto r = statement->accept({
            func_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Statement analysis failed in function '" +
                    name_token.value + "'",
                name_token.line
            );

            return r;
        }

        for (auto& rnd : r.return_nodes) {
            if (rnd != nullptr) {
                if (!are_types_compatible(
                        rnd->return_type,
                        return_type_token.value
                    )) {

                    emit_event(
                        Context,
                        DevTools::EventsT::Analysis,
                        "Return type mismatch in function '" +
                            name_token.value + "': expected '" +
                            return_type_token.value + "', got '" +
                            rnd->return_type + "'",
                        name_token.line
                    );

                    auto err = Errors::UserFunctionError(
                        name_token.value,
                        name_token.line,
                        name_token.column,
                        name_token.value.size()
                    );

                    ReturnResult<bool> re = {
                        err.return_type_error(
                            return_type_token.value,
                            rnd->return_type
                        ),
                        false,
                        false
                    };

                    re.error_info = err.info;
                    return re;
                }

                is_there_return = true;
            }
        }
    }


    // Check for missing return
    if (return_type_token.value != "void" &&
        !is_there_return) {

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Function '" + name_token.value +
                "' has no valid RETURN statement",
            name_token.line
        );

        auto err = Errors::UserFunctionError(
            name_token.value,
            name_token.line,
            name_token.column,
            name_token.value.size()
        );

        ReturnResult<bool> re = {
            err.missing_return_error(),
            false,
            false
        };

        re.error_info = err.info;
        return re;
    }


    // Create function in scope
    auto func_ptr = Context.ParentScope->add_function(
        name_token.value,
        return_type_token.value,
        std::move(MList),
        is_function_any
    );

    func_ptr->type =
        Scopes::SymbolTableTypes::FunctionsTypes::Inside;


    // Create a new proxy function node
    NodesListT statements_copy;

    for (auto& stm : statements) {
        if (stm == nullptr) {
            statements_copy.push_back(nullptr);
            continue;
        }

        auto t = stm->clone();
        std::unique_ptr<ASTNode> t_ptr(t);

        statements_copy.push_back(std::move(t_ptr));
    }

    auto proxy_node = new UserProxyFunctionNode(file, END_token, std::move(statements_copy),
        return_type_token.value
    );

    func_ptr->user_function = proxy_node;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Function '" + name_token.value +
            "' passed analysis and was registered successfully",
        name_token.line
    );

    return {"", true, true};
}


// Execute node
ReturnResult<Value>
UserFunctionNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    return_type = "void";

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Registering function '" + name_token.value + "'",
        name_token.line
    );

    // Create proxy node
    auto proxy_node = new UserProxyFunctionNode(file, END_token, std::move(statements),
        return_type_token.value
    );

    // Create arguments list
    std::unordered_map<
        std::string,
        Scopes::SymbolTableTypes::Argument
    > MList;

    for (auto& argument : arguments_list) {
        if (argument == nullptr)
            continue;

        // Execute argument
        auto r = argument->exec(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Failed to evaluate parameters for function '" +
                    name_token.value + "'",
                argument->name_token.line
            );

            delete proxy_node;
            return r;
        }

        // Add argument to arguments list
        MList[argument->name_token.value] = {
            argument->type_token.value,
            true,
            argument->default_value_node == nullptr,
            argument->is_argument_any,
            argument->default_value
        };
    }

    // Create function in scope
    auto func_ptr = Context.ParentScope->add_function(
        name_token.value,
        return_type_token.value,
        std::move(MList),
        is_function_any
    );

    func_ptr->type =
        Scopes::SymbolTableTypes::FunctionsTypes::Inside;

    func_ptr->user_function = proxy_node;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Function '" + name_token.value +
            "' registered successfully",
        name_token.line
    );

    return {"", true, std::monostate{}};
}


// Copy class
ASTNode*
UserFunctionNode::clone() {
    argumentsList new_arguments_list;
    NodesListT new_statements;

    for (auto& m : arguments_list) {
        if (m == nullptr) {
            new_arguments_list.push_back(nullptr);
            continue;
        }

        std::unique_ptr<UserFunctionArgumentNode> new_argument(
            dynamic_cast<UserFunctionArgumentNode*>(m->clone())
        );

        new_arguments_list.push_back(std::move(new_argument));
    }

    for (auto& s : statements) {
        if (s == nullptr) {
            new_statements.push_back(nullptr);
            continue;
        }

        std::unique_ptr<ASTNode> new_stm(s->clone());
        new_statements.push_back(std::move(new_stm));
    }

    return new UserFunctionNode(file, END_token, name_token,
        return_type_token,
        std::move(new_arguments_list),
        std::move(new_statements),
        is_function_any
    );
}

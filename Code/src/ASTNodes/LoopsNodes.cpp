// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/LoopsNodes.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>
#include <string>

// ==================================================================
// Runtime Tracker Helper
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
// Classes of break and continue nodes
// ==================================================================

// Constructor
BreakLoopNode::BreakLoopNode(const std::string& FilePath,
    const Token& END_token,
    Token& t_
): BreakT(t_),
    ASTNode(FilePath, END_token) {}

// Constructor
ContinueLoopNode::ContinueLoopNode(const std::string& FilePath,
    const Token& END_token,
    Token& t_
): ContinueT(t_),
    ASTNode(FilePath, END_token) {}

// Get the string to print
std::string
BreakLoopNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Break \n";

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
BreakLoopNode::get_json() {
    nlohmann::json result;

    result["type"] = "Break";

    return result;
}

// Get the string to print
std::string
ContinueLoopNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Continue \n";

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
ContinueLoopNode::get_json() {
    nlohmann::json result;

    result["type"] = "Continue";

    return result;
}

// Get the type of node
ASTNodesTypes
BreakLoopNode::NType() {
    return NT__BreakLoopNode;
}

// Get the type of node
ASTNodesTypes
ContinueLoopNode::NType() {
    return NT__ContinueLoopNode;
}

// ==================================================================
// Break node: semantic analysis
// ==================================================================

ReturnResult<bool>
BreakLoopNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing BREAK statement",
        BreakT.line
    );

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "BREAK statement analysis completed",
        BreakT.line
    );

    return {"", true, true, ExecState::Break};
}

// ==================================================================
// Continue node: semantic analysis
// ==================================================================

ReturnResult<bool>
ContinueLoopNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing CONTINUE statement",
        ContinueT.line
    );

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "CONTINUE statement analysis completed",
        ContinueT.line
    );

    return {"", true, true, ExecState::Continue};
}

// ==================================================================
// Break node: runtime execution
// ==================================================================

ReturnResult<Value>
BreakLoopNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "BREAK encountered; exiting the current loop",
        BreakT.line
    );

    return {
        "",
        true,
        std::monostate{},
        ExecState::Break
    };
}

// ==================================================================
// Continue node: runtime execution
// ==================================================================

ReturnResult<Value>
ContinueLoopNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "CONTINUE encountered; skipping the remaining loop body",
        ContinueT.line
    );

    return {
        "",
        true,
        std::monostate{},
        ExecState::Continue
    };
}

// ==================================================================
// Clone break and continue nodes
// ==================================================================

ASTNode*
BreakLoopNode::clone() {
    return new BreakLoopNode(file, END_token, BreakT);
}

ASTNode*
ContinueLoopNode::clone() {
    return new ContinueLoopNode(file, END_token, ContinueT);
}

// ==================================================================
// While loop node functions
// ==================================================================

// Constructor
WhileLoopsNode::WhileLoopsNode(const std::string& FilePath,
    const Token& END_token,
    Token& FirstToken,
    Node& condition,
    NodesL& nodes_list
):
    condition_node(std::move(condition)),
    FirstToken(FirstToken),
    Statments(std::move(nodes_list)),
    ASTNode(FilePath, END_token) {
    return_type = "void";
}

// Get the string to print
std::string
WhileLoopsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "While Loop \n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Condition: \n"
       << condition_node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Statments: \n";

    for (auto& stm : Statments)
        ss << stm->get_str(level + 2);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
WhileLoopsNode::get_json() {
    nlohmann::json result;

    result["type"] = "WhileLoop";
    result["condition"] = condition_node->get_json();

    result["statements"] = nlohmann::json::array();

    for (const auto& stm : Statments)
        result["statements"].push_back(stm->get_json());

    return result;
}

// Get the type of node
ASTNodesTypes
WhileLoopsNode::NType() {
    return NT__WhileLoopsNode;
}

// ==================================================================
// Clone while loop node
// ==================================================================

ASTNode*
WhileLoopsNode::clone() {
    Node new_condition(
        condition_node ? condition_node->clone() : nullptr
    );

    NodesL new_stms_list;

    for (auto& stm : Statments) {
        Node new_stm(stm ? stm->clone() : nullptr);
        new_stms_list.push_back(std::move(new_stm));
    }

    return new WhileLoopsNode(file, END_token, FirstToken,
        new_condition,
        new_stms_list
    );
}

// ==================================================================
// While loop: semantic analysis
// ==================================================================

ReturnResult<bool>
WhileLoopsNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting WHILE loop semantic analysis",
        FirstToken.line
    );

    // Set return type to void
    return_type = "void";

    // Verify condition node
    if (!condition_node) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "WHILE condition node is null",
            FirstToken.line
        );

        return {
            "WHILE condition node is null",
            false,
            false
        };
    }

    auto r = condition_node->accept(Context);

    if (!r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "WHILE condition analysis failed",
            FirstToken.line
        );

        return r;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "WHILE condition analysis completed",
        FirstToken.line
    );

    // Verify condition type
    if (condition_node->return_type != "bool") {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "WHILE condition type mismatch: expected bool",
            FirstToken.line
        );

        auto err = Errors::TypeError(
            "bool",
            condition_node->return_type,
            FirstToken.line,
            FirstToken.column,
            FirstToken.value.size()
        );

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;

        return re;
    }

    // Create the returning nodes list
    std::vector<ASTNode*> return_nodes;

    // Create a specific scope for statements
    auto while_scoupe =
        std::make_unique<Scopes::Scope>(Context.ParentScope);

    // Verify all statements
    for (auto& smt : Statments) {
        if (!smt) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Null statement found in WHILE loop",
                FirstToken.line
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing WHILE loop statement",
            FirstToken.line
        );

        auto statement_result = smt->accept({
            while_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!statement_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "WHILE loop statement analysis failed",
                FirstToken.line
            );

            return statement_result;
        }

        if (statement_result.state == ExecState::Return) {
            for (auto rptr : statement_result.return_nodes)
                return_nodes.push_back(rptr);
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "WHILE loop semantic analysis completed",
        FirstToken.line
    );

    return {
        "",
        true,
        true,
        return_nodes.empty()
            ? ExecState::Normal
            : ExecState::Return,
        std::move(return_nodes)
    };
}

// ==================================================================
// While loop: runtime execution
// ==================================================================

ReturnResult<Value>
WhileLoopsNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Starting WHILE loop execution",
        FirstToken.line
    );

    return_type = "void";

    if (!condition_node) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Cannot execute WHILE loop: condition node is null",
            FirstToken.line
        );

        return {
            "WHILE condition node is null",
            false,
            std::monostate{}
        };
    }

    ReturnResult<Value> condition_result;

    {
        // Copy condition node
        auto condition_node_cp_raw_ptr = condition_node->clone();
        Node condition_node_cp(condition_node_cp_raw_ptr);

        // Execute condition
        condition_result = condition_node_cp->exec(Context);

        if (!condition_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "WHILE condition execution failed",
                FirstToken.line
            );

            return condition_result;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        condition_result.value == Value(true)
            ? "WHILE condition evaluated to true"
            : "WHILE condition evaluated to false",
        FirstToken.line
    );

    // Execute statements while condition is true
    bool steel_running = true;

    while (
        condition_result.value == Value(true) &&
        steel_running
    ) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Entering WHILE loop iteration",
            FirstToken.line
        );

        // Create a specific scope for this iteration
        auto while_scoupe =
            std::make_unique<Scopes::Scope>(Context.ParentScope);

        for (auto& stm : Statments) {
            if (!stm)
                continue;

            // Copy statement node
            auto stm_raw_ptr = stm->clone();
            Node stm_ptr(stm_raw_ptr);

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Executing WHILE loop statement",
                FirstToken.line
            );

            // Execute statement
            auto r = stm_ptr->exec({
                while_scoupe.get(),
                Context.Debugger,
            Context.FilePath
            });

            if (!r.success) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "WHILE loop statement execution failed",
                    FirstToken.line
                );

                return r;
            }

            // Return from function
            if (r.state == ExecState::Return) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "RETURN propagated from WHILE loop",
                    FirstToken.line
                );

                return r;
            }

            // Continue to the next iteration
            if (r.state == ExecState::Continue) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "CONTINUE propagated by WHILE loop",
                    FirstToken.line
                );

                break;
            }

            // Exit the loop
            if (r.state == ExecState::Break) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "BREAK propagated by WHILE loop",
                    FirstToken.line
                );

                steel_running = false;
                break;
            }
        }

        if (steel_running) {
            // Copy condition node
            auto condition_node_cp_raw_ptr = condition_node->clone();
            Node condition_node_cp(condition_node_cp_raw_ptr);

            // Evaluate condition again
            condition_result = condition_node_cp->exec(Context);

            if (!condition_result.success) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "WHILE condition reevaluation failed",
                    FirstToken.line
                );

                return condition_result;
            }

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                condition_result.value == Value(true)
                    ? "WHILE condition evaluated to true"
                    : "WHILE condition evaluated to false",
                FirstToken.line
            );
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "WHILE loop execution completed",
        FirstToken.line
    );

    // Delete all statements
    Statments.clear();

    // Delete condition node
    condition_node = nullptr;

    return {
        "",
        true,
        std::monostate{}
    };
}

// ==================================================================
// Class of for loops
// ==================================================================

// Constructor
ForLoopsNode::ForLoopsNode(const std::string& FilePath,
    const Token& END_token,
    Component& Initialization,
    Component& Condition,
    Component& Update,
    NodesL& statments
):
    Initialization(std::move(Initialization)),
    Condition(std::move(Condition)),
    Update(std::move(Update)),
    Statments(std::move(statments)),
    ASTNode(FilePath, END_token) {
    return_type = "void";
}

// Get the string to print
std::string
ForLoopsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "For Loop \n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Initialization: \n"
       << Initialization.node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Condition: \n"
       << Condition.node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Update: \n"
       << Update.node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Statments: \n";

    for (auto& stm : Statments)
        ss << stm->get_str(level + 2);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
ForLoopsNode::get_json() {
    nlohmann::json result;

    result["type"] = "ForLoop";

    result["initialization"] = Initialization.node->get_json();
    result["condition"] = Condition.node->get_json();
    result["update"] = Update.node->get_json();

    result["statements"] = nlohmann::json::array();

    for (const auto& stm : Statments)
        result["statements"].push_back(stm->get_json());

    return result;
}

// Get the type of node
ASTNodesTypes
ForLoopsNode::NType() {
    return NT__ForLoopsNode;
}

// ==================================================================
// Clone for loop node
// ==================================================================

ASTNode*
ForLoopsNode::clone() {
    // Create new unique_ptrs for components
    Node Initialization_cp(
        Initialization.node
            ? Initialization.node->clone()
            : nullptr
    );

    Node Condition_cp(
        Condition.node
            ? Condition.node->clone()
            : nullptr
    );

    Node Update_cp(
        Update.node
            ? Update.node->clone()
            : nullptr
    );

    // Create component objects
    Component new_Initialization = {
        Initialization.first_token,
        std::move(Initialization_cp)
    };

    Component new_Condition = {
        Condition.first_token,
        std::move(Condition_cp)
    };

    Component new_Update = {
        Update.first_token,
        std::move(Update_cp)
    };

    // Copy statements
    NodesL new_stms_list;

    for (auto& stm : Statments) {
        Node new_stm(stm ? stm->clone() : nullptr);
        new_stms_list.push_back(std::move(new_stm));
    }

    // Create and return the new node
    return new ForLoopsNode(file, END_token, new_Initialization,
        new_Condition,
        new_Update,
        new_stms_list
    );
}

// ==================================================================
// For loop: semantic analysis
// ==================================================================

ReturnResult<bool>
ForLoopsNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting FOR loop semantic analysis",
        Initialization.first_token.line
    );

    // Create a scope for the for loop
    auto for_scoupe =
        std::make_unique<Scopes::Scope>(Context.ParentScope);

    // Create the list of components
    std::vector<Component*> standard_components = {
        &Initialization,
        &Condition,
        &Update
    };

    // Verify components
    for (auto& comp : standard_components) {
        if (!comp->node) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "FOR loop contains a null component",
                comp->first_token.line
            );

            return {
                "FOR loop contains a null component",
                false,
                false
            };
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing FOR loop component",
            comp->first_token.line
        );

        auto r = comp->node->accept({
            for_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "FOR loop component analysis failed",
                comp->first_token.line
            );

            return r;
        }
    }

    // Verify condition node
    if (Condition.node->return_type != "bool") {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "FOR condition type mismatch: expected bool",
            Condition.first_token.line
        );

        auto err = Errors::TypeError(
            "bool",
            Condition.node->return_type,
            Condition.first_token.line,
            Condition.first_token.column,
            Condition.first_token.value.size(),
            "Condition of for loop need to be bolean !!"
        );

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "FOR loop components validated",
        Initialization.first_token.line
    );

    // Create the returning nodes list
    std::vector<ASTNode*> return_nodes;

    // Verify all statements
    for (auto& stm : Statments) {
        if (!stm) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Null statement found in FOR loop",
                Initialization.first_token.line
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing FOR loop statement",
            Initialization.first_token.line
        );

        auto r = stm->accept({
            for_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "FOR loop statement analysis failed",
                Initialization.first_token.line
            );

            return r;
        }

        if (r.state == ExecState::Return) {
            for (auto rptr : r.return_nodes)
                return_nodes.push_back(rptr);
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "FOR loop semantic analysis completed",
        Initialization.first_token.line
    );

    return {
        "",
        true,
        true,
        return_nodes.empty()
            ? ExecState::Normal
            : ExecState::Return,
        std::move(return_nodes)
    };
}

// ==================================================================
// For loop: runtime execution
// ==================================================================

ReturnResult<Value>
ForLoopsNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Starting FOR loop execution",
        Initialization.first_token.line
    );

    // Create a scope for the for loop
    auto for_scoupe =
        std::make_unique<Scopes::Scope>(Context.ParentScope);

    // Initialize condition result
    ReturnResult<Value> condition_result;

    // Run initialization
    if (!Initialization.node) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "FOR initialization node is null",
            Initialization.first_token.line
        );

        return {
            "FOR initialization node is null",
            false,
            std::monostate{}
        };
    }

    {
        auto r = Initialization.node->exec({
            for_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "FOR initialization failed",
                Initialization.first_token.line
            );

            return r;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "FOR initialization completed",
        Initialization.first_token.line
    );

    // Run the condition for the first time
    if (!Condition.node) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "FOR condition node is null",
            Condition.first_token.line
        );

        return {
            "FOR condition node is null",
            false,
            std::monostate{}
        };
    }

    {
        auto condition_cp_raw_ptr = Condition.node->clone();
        Node condition_cp(condition_cp_raw_ptr);

        condition_result = condition_cp->exec({
            for_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!condition_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Initial FOR condition evaluation failed",
                Condition.first_token.line
            );

            return condition_result;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        condition_result.value == Value(true)
            ? "FOR condition evaluated to true"
            : "FOR condition evaluated to false",
        Condition.first_token.line
    );

    // Execute statements while the condition is true
    bool steel_running = true;

    while (
        condition_result.value == Value(true) &&
        steel_running
    ) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Entering FOR loop iteration",
            Initialization.first_token.line
        );

        // Create a scope for this iteration
        auto while_scoupe =
            std::make_unique<Scopes::Scope>(for_scoupe.get());

        for (auto& stm : Statments) {
            if (!stm)
                continue;

            // Copy statement node
            auto stm_raw_ptr = stm->clone();
            Node stm_ptr(stm_raw_ptr);

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Executing FOR loop statement",
                Initialization.first_token.line
            );

            // Execute statement
            auto r = stm_ptr->exec({
                while_scoupe.get(),
                Context.Debugger,
            Context.FilePath
            });

            if (!r.success) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "FOR loop statement execution failed",
                    Initialization.first_token.line
                );

                return r;
            }

            // Return from function
            if (r.state == ExecState::Return) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "RETURN propagated from FOR loop",
                    Initialization.first_token.line
                );

                return r;
            }

            // Continue to the next iteration
            if (r.state == ExecState::Continue) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "CONTINUE propagated by FOR loop",
                    Initialization.first_token.line
                );

                break;
            }

            // Exit the loop
            if (r.state == ExecState::Break) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "BREAK propagated by FOR loop",
                    Initialization.first_token.line
                );

                steel_running = false;
                break;
            }
        }

        // Run update and reevaluate condition
        if (steel_running) {
            if (!Update.node) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "FOR update node is null",
                    Update.first_token.line
                );

                return {
                    "FOR update node is null",
                    false,
                    std::monostate{}
                };
            }

            {
                // Copy update node
                auto update_node_cp_raw_ptr = Update.node->clone();
                Node update_node_cp(update_node_cp_raw_ptr);

                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "Executing FOR update",
                    Update.first_token.line
                );

                // Execute update
                auto update_result = update_node_cp->exec({
                    for_scoupe.get(),
                    Context.Debugger,
            Context.FilePath
                });

                if (!update_result.success) {
                    emit_event(
                        Context,
                        DevTools::EventsT::Runtime,
                        "FOR update execution failed",
                        Update.first_token.line
                    );

                    return update_result;
                }
            }

            // Copy condition node
            auto condition_node_cp_raw_ptr = Condition.node->clone();
            Node condition_node_cp(condition_node_cp_raw_ptr);

            // Evaluate condition again
            condition_result = condition_node_cp->exec({
                for_scoupe.get(),
                Context.Debugger,
            Context.FilePath
            });

            if (!condition_result.success) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "FOR condition reevaluation failed",
                    Condition.first_token.line
                );

                return condition_result;
            }

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                condition_result.value == Value(true)
                    ? "FOR condition evaluated to true"
                    : "FOR condition evaluated to false",
                Condition.first_token.line
            );
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "FOR loop execution completed",
        Initialization.first_token.line
    );

    // Delete all statements
    Statments.clear();

    // Delete all component nodes
    Initialization.node = nullptr;
    Condition.node = nullptr;
    Update.node = nullptr;

    return {
        "",
        true,
        std::monostate{}
    };
}

// ==================================================================
// Include necessary headers
// ==================================================================

// == Libs ==
#include <sstream>
#include <string>

// == Locals ==
#include "detail/ast/ast_node.hpp"

// ==================================================================
// Runtime Tracker Helper
// ==================================================================

namespace vltc {
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
            Context.FilePath.string()
        });
    }
}

// Constructure of ASTNode that creates the file
ASTNode::ASTNode(const Token& END_token)
    : file(END_token.FilePath), END_token(END_token) {}

void ASTNode::emit_exec_end_event(const NodesContext& Context) const {
    if (Context.Debugger) {
        Context.Debugger->emit({
            DevTools::EventsT::Debug,
            "AST node execution finished",
            END_token.line,
            Context.ParentScope,
            file.string()
        });
    }
}

// ==================================================================
// Module node functions
// ==================================================================

// Constructor
ModuleNode::ModuleNode(
    const Token& END_token,
    StatmentsT& s_,
    Token& First_token
): statements(std::move(s_)),
   FToken(First_token),
   ASTNode(END_token) {
    return_type = "void";
}

// Get string to print
std::string
ModuleNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Program:\n";

    for (auto& statement : statements) {
        if (statement.node)
            ss << statement.node->get_str(level + 1);
    }

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
ModuleNode::get_json() {
    nlohmann::json result;

    result["type"] = "Program";
    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement.node)
            result["statements"].push_back(statement.node->get_json());
    }

    return result;
}

// Get the node type
ASTNodesTypes
ModuleNode::NType() {
    return NT__ModuleNode;
}

// ==================================================================
// Module node: runtime execution
// ==================================================================

ReturnResult<Value>
ModuleNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Info,
        "Starting program execution",
        FToken.line
    );

    ReturnResult<Value> r;

    std::size_t statement_index = 0;

    for (auto& stmt : statements) {
        ++statement_index;

        if (!stmt.node) {
            emit_event(
                Context,
                DevTools::EventsT::Debug,
                "Skipping null statement #" +
                    std::to_string(statement_index),
                stmt.first_token.line
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Debug,
            "Executing program statement #" +
                std::to_string(statement_index),
            stmt.first_token.line
        );

        r = stmt.node->exec(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Error,
                "Fail in executing program statement #" +
                    std::to_string(statement_index) + ": " + r.Message,
                stmt.first_token.line
            );

            return r;
        }

        // Preserve the original ownership behavior.
        stmt.node = nullptr;
    }

    emit_event(
        Context,
        DevTools::EventsT::Info,
        "Program execution completed successfully",
        END_token.line
    );

    return {"", true, std::monostate{}};
}

// ==================================================================
// Module node: semantic analysis
// ==================================================================

ReturnResult<bool>
ModuleNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting program semantic analysis",
        FToken.line
    );

    ReturnResult<bool> r;

    std::size_t statement_index = 0;

    for (auto& stmt : statements) {
        ++statement_index;

        if (!stmt.node) {
            emit_event(
                Context,
                DevTools::EventsT::Debug,
                "Skipping null statement #" +
                    std::to_string(statement_index),
                stmt.first_token.line
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing program statement #" +
                std::to_string(statement_index),
            stmt.first_token.line
        );

        r = stmt.node->accept(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Error,
                "Fail in analyzing program statement #" +
                    std::to_string(statement_index) + ": " + r.Message,
                stmt.first_token.line
            );

            return r;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Program semantic analysis completed successfully",
        END_token.line
    );

    return {"", true, true};
}

// ==================================================================
// Clone module node
// ==================================================================

ASTNode*
ModuleNode::clone() {
    StatmentsT new_statments;

    for (auto& s : statements) {
        if (!s.node) {
            new_statments.push_back({nullptr});
            continue;
        }

        std::unique_ptr<ASTNode> statment_c(s.node->clone());
        new_statments.push_back({std::move(statment_c),s.first_token});
    }

    return new ModuleNode(END_token, new_statments, FToken);
}

} // namespace vltc

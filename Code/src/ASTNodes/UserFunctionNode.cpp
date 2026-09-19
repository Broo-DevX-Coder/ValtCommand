// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/UserFunctionNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// User Function method node functions
// ==================================================================

// Constructure
UserFunctionMethodNode::UserFunctionMethodNode(
    Token& nameT, 
    Token& typeT, 
    Token& default_value_FT,
    std::unique_ptr<ASTNode> default_value_N,
    bool is_method_any
): 
    name_token(nameT),
    type_token(typeT),
    default_value_node(std::move(default_value_N)),
    default_value_ftoekn(default_value_FT),
    is_method_any(is_method_any) {}

// Get string for print
std::string 
UserFunctionMethodNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Method " << name_token.value << ":\n";

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "| " << "type: " << type_token.value << "\n";

    if (default_value_node != nullptr) {
        for (int i=0;i<level;i++)
            ss << "|  ";
        ss << "| " << "default value: " << "\n";

        ss <<  default_value_node->get_str(level+1);
    }

    return ss.str();
}

// get type of node
ASTNodesTypes UserFunctionMethodNode::NType() {
    return NT__UserFunctionMethodNode;
}

// Verify method
ReturnResult<bool> 
UserFunctionMethodNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = type_token.value;
    // Verifi defualt value node
    if (default_value_node != nullptr) {
        auto result = default_value_node->accept(ParentScope);

        if (!result.success)
            return {result.Message,false,false};
        
        if (!are_types_compatible(default_value_node->return_type,return_type))
            return {
                Errors::TypeError(
                    return_type, default_value_node->return_type,
                    default_value_ftoekn.line,
                    default_value_ftoekn.column
                ).msg, false, false
            };
    }
    return {"",true,true};
}

// Execute node
ReturnResult<Value> 
UserFunctionMethodNode::exec(
    Scopes::Scope* ParentScope
) {
    if (default_value_node != nullptr) {
        auto vnode_r = default_value_node->exec(ParentScope);
        if (!vnode_r.success)
            return {vnode_r.Message,false,std::monostate{}};
        
        default_value=reconsiliation_int_float(type_token.value,vnode_r.value);

        default_value_node = nullptr;
    }

    return {"",true,std::monostate{}};
} 

// Copy class
ASTNode* 
UserFunctionMethodNode::clone() {
    std::unique_ptr<ASTNode> new_defualt_value_node(
        default_value_node==nullptr?
            nullptr :
            default_value_node->clone()
    );

    return new UserFunctionMethodNode(
        name_token,
        type_token,
        default_value_ftoekn,
        default_value_node==nullptr?
            nullptr :
            std::move(new_defualt_value_node),
        is_method_any
    );
}

// ==================================================================
// User Function's return node
// ==================================================================

// Constructure
UserFunctionReturnNode::UserFunctionReturnNode(
    std::unique_ptr<ASTNode> value_node
): VNode(std::move(value_node)) {}

// get string to print
std::string 
UserFunctionReturnNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Return value:\n";

    ss <<  VNode->get_str(level+1);

    return ss.str();
}

// Type of node
ASTNodesTypes 
UserFunctionReturnNode::NType() {
    return NT__UserFunctionReturnNode;
}

// Verifi befor running
ReturnResult<bool> 
UserFunctionReturnNode::accept(
    Scopes::Scope* ParentScope
) {
    auto r = VNode->accept(ParentScope);
    if (!r.success) return {r.Message,false,false,ExecState::Return};

    return_type = VNode->return_type;

    return {"",true,true,ExecState::Return,this};
}

// Execute node and get value
ReturnResult<Value> 
UserFunctionReturnNode::exec(
    Scopes::Scope* ParentScope
) {
    auto vnode_r = VNode->exec(ParentScope);
    if (!vnode_r.success) return {vnode_r.Message,false,std::monostate{},ExecState::Return,this};

    VNode = nullptr;

    return {"",true,std::monostate(),ExecState::Return,this,vnode_r.value};
}

// Copy class
ASTNode* 
UserFunctionReturnNode::clone() {
    std::unique_ptr<ASTNode> new_VNode(VNode->clone());

    return new UserFunctionReturnNode(std::move(new_VNode));
}

// ==================================================================
// User Proxy Function node
// ==================================================================

// constructure
UserProxyFunctionNode::UserProxyFunctionNode(
    NodesListT statements,
    const std::string& type
):
    statements(std::move(statements)){
    return_type=type;
}

// get str to print
std::string
UserProxyFunctionNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "User Function" << "\n";

    for (auto& statement: statements)
        ss << statement->get_str(level+1);

    return ss.str();
}


// Get type
ASTNodesTypes 
UserProxyFunctionNode::NType() {
    return NT__UserProxyFunctionNode;
}

// Verify befor runnig
ReturnResult<bool> 
UserProxyFunctionNode::accept(
    Scopes::Scope* ParentScope
) {
    return {"",true,true};
}

// Execute node
ReturnResult<Value> 
UserProxyFunctionNode::exec(
    Scopes::Scope* ParentScope
) {
    for (auto& statement: statements) {
        auto r = statement->exec(ParentScope);
        if (!r.success) 
            return r;
        if (r.state == ExecState::Return){
            return {"",true,reconsiliation_int_float(return_type,r.return_data)};
        }
        statement = nullptr;
    }
    return {"",true,std::monostate()};
}

// Copy class
ASTNode* 
UserProxyFunctionNode::clone() {
    NodesListT new_statments;

    for (auto& s: statements) {
        std::unique_ptr<ASTNode> stm(s->clone());
        new_statments.push_back(std::move(stm));
    }

    return new UserProxyFunctionNode(std::move(new_statments),return_type);
}

// ==================================================================
// User Function node
// ==================================================================

// Constructure
UserFunctionNode::UserFunctionNode(
    Token& nameT, 
    Token& typeT, 
    MethodsList methods_list, 
    NodesListT statements,
    bool is_any
):
    name_token(nameT),
    return_type_token(typeT),
    methods_list(std::move(methods_list)),
    statements(std::move(statements)),
    is_function_any(is_any) {}


// get str to print
std::string
UserFunctionNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "User Function " << name_token.value << ":\n";

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "|  " << "type: " << return_type_token.value << "\n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Methods:" << "\n";

    for (auto& method: methods_list)
        ss << method->get_str(level+2);

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Statments:" << "\n";

    for (auto& statement: statements)
        ss << statement->get_str(level+2);

    return ss.str();
}

// Get node type
ASTNodesTypes
UserFunctionNode::NType() {
    return NT__UserFunctionNode;
}

// verify node
ReturnResult<bool> 
UserFunctionNode::accept(
    Scopes::Scope* ParentScope
) {

    return_type = "void";
    bool is_there_return = false;


    // When find the function in same scoupe
    auto search_result = ParentScope->search_function(name_token);
    if (search_result.success)  {
        if (search_result.value->scope_id == ParentScope->get_id())
            return {
                Errors::UserFunctionError(
                    name_token.value,
                    name_token.line,
                    name_token.column
                ).function_already_defined(), false, false
            };
    }

    // Create a new scope for verifi
    auto func_scoupe = std::make_unique<Scopes::Scope>(ParentScope);

    // Create methods list to add function in Scope
    std::unordered_map<std::string, Scopes::SymbolTableTypes::Method> MList;

    // Verifi all args
    for (auto& method: methods_list) {
        auto r = method->accept(ParentScope);
        if (!r.success) 
            return r;
        
        // Put the method in scope
        Value arg_v = std::monostate();
        func_scoupe->add_var(
            method->name_token.value,
            method->type_token.value,
            arg_v,
            true
        );

        // Put the method in methods list
        MList[method->name_token.value] = {
            method->type_token.value,
            true,
            method->default_value_node==nullptr,
            method->is_method_any,
            std::monostate{}
        };
    }


    // Verifi all statments
    for (auto& statement: statements) {
        auto r = statement->accept(func_scoupe.get());
        if (!r.success) 
            return r;
        
        if (r.return_node != nullptr) {
            if (!are_types_compatible(r.return_node->return_type,return_type_token.value)){
                return {Errors::UserFunctionError(
                    name_token.value,
                    name_token.line,
                    name_token.column
                    ).return_type_error(return_type_token.value, r.return_node->return_type),false,false
                };
            }
            is_there_return = true;
        }
    }


    if (return_type_token.value != "void" && !is_there_return)
        return {
            Errors::UserFunctionError(
                name_token.value,
                name_token.line,
                name_token.column
            ).missing_return_statement(), false, false
        };

    // create function in scope
    auto func_ptr = ParentScope->add_function(
        name_token.value,
        return_type_token.value,
        std::move(MList),
        is_function_any
    );
    func_ptr->type = Scopes::SymbolTableTypes::FunctionsTypes::Inside;

    return {"",true,true};
}

// Execute node
ReturnResult<Value> 
UserFunctionNode::exec(
    Scopes::Scope* ParentScope
) {
    // Create proxy node
    auto proxy_node = new UserProxyFunctionNode(std::move(statements),return_type_token.value);

    // Create methods list
    std::unordered_map<std::string, Scopes::SymbolTableTypes::Method> MList;
    for(auto& method: methods_list) {

        // execute method
        auto r = method->exec(ParentScope);
        if (!r.success) return r;

        // Add method to methods list
        MList[method->name_token.value] = {
            method->type_token.value,
            true,
            method->default_value_node==nullptr,
            method->is_method_any,
            method->default_value
        };
    }

    // create function in scope
    auto func_ptr = ParentScope->add_function(
        name_token.value,
        return_type_token.value,
        std::move(MList),
        is_function_any
    );

    func_ptr->type = Scopes::SymbolTableTypes::FunctionsTypes::Inside;
    func_ptr->user_function = proxy_node;

    return {"",true,std::monostate()};
}

// Copy class
ASTNode* 
UserFunctionNode::clone() {
    MethodsList new_methods_list;
    NodesListT new_statements;

    for (auto& m: methods_list) {
        std::unique_ptr<UserFunctionMethodNode> new_method(dynamic_cast<UserFunctionMethodNode*>(m->clone()));
        new_methods_list.push_back(std::move(new_method));
    }

    for (auto& s: statements) {
        std::unique_ptr<ASTNode> new_stm(s->clone());
        new_statements.push_back(std::move(new_stm));
    }

    return new UserFunctionNode(
        name_token,
        return_type_token,
        std::move(new_methods_list),
        std::move(new_statements),
        is_function_any
    );
}
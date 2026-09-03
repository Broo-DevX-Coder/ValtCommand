// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <sstream>
#include <cstdint>
#include <algorithm>

// == Locals ==
#include "ASTNodes/FunctionCallNode.hpp"
#include "Errors.hpp"

// ==================================================================
// Function Call Argument Node
// ==================================================================

// Constructure
FunctionCallArgumentNode::FunctionCallArgumentNode(
    std::string& Aname,
    Token TToken,
    std::unique_ptr<ASTNode> Value_node
): 
    name(Aname),
    type(TToken.value),
    TypeToken(TToken),
    VNode(std::move(Value_node))
{
    return_type = type;
}

// get str to print
std::string
FunctionCallArgumentNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Argument " << name << ":" << "\n";

    ss <<  VNode->get_str(level+1);

    return ss.str();
}

// Get the node type
ASTNodesTypes 
FunctionCallArgumentNode::NType() {
    return NT__FunctionCallArgumentNode;
}

// Verifi the node 
ReturnResult<bool>
FunctionCallArgumentNode::accept(
    Scopes::Scope* ParentScope
) {
    auto vnode_ar = VNode->accept(ParentScope);
    if (!vnode_ar.success) return {vnode_ar.Message,false,false};

    if (!are_types_compatible(VNode->return_type,type)){
        return {
            Errors::TypeError(
                type,VNode->return_type,
                TypeToken.line,
                TypeToken.column
            ).msg,false,false
        };
    }

    return {"",true,true};
}

// Execute node
ReturnResult<Value> 
FunctionCallArgumentNode::exec(
    Scopes::Scope* ParentScope
) {
    auto vnode_r = VNode->exec(ParentScope);
    if (!vnode_r.success)
        return {vnode_r.Message,false,std::monostate{}};
    
    value = vnode_r.value;
    VNode = nullptr;

    return {"",true,std::monostate{}};
}

// ==================================================================
// Function Call Node
// ==================================================================

// Constructure
FunctionCallNode::FunctionCallNode(
    std::string Fname, 
    ArgsT& Args_list, 
    Token n_token
): 
    name(Fname),
    arguments(std::move(Args_list)),
    NameToken(n_token)
{}

// get str to print
std::string
FunctionCallNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "FunctionCall " << name << ":" << "\n";

    for (auto& arg: arguments)
        ss << arg->get_str(level+1);

    return ss.str();
}

// Get the node type
ASTNodesTypes 
FunctionCallNode::NType() {
    return NT__FunctionCallNode;
}

// Execute node
ReturnResult<bool> 
FunctionCallNode::accept(
    Scopes::Scope* ParentScope
) {
    auto search_result = ParentScope->search_function(NameToken);
    if (!search_result.success){
        return {
            search_result.Message,
            false,false
        };
    }
    return_type = search_result.value->return_type;
    func = search_result.value;

    auto error_obj = Errors::ArgumentError(
        name,
        NameToken.line,
        NameToken.column
    );
    if (!func->is_sepport_any_methods_) {

        if (arguments.size() > func->methods.size()) {
            return {
                error_obj.too_many_arguments(func->methods.size(),arguments.size()),
                false,false
            };

        } else {
            for (auto& ar: arguments) {
                if (!func->methods.contains(ar->name)) {
                    return {
                        error_obj.unexpected_argument(ar->name),
                        false,false
                    };
                }
            }

            for (auto& [mt_n,mt]: func->methods) {
                if (mt.is_required) {
                    auto it = std::find_if(
                        arguments.begin(),
                        arguments.end(),
                        [mt_n](const std::unique_ptr<FunctionCallArgumentNode>& arg){
                            return arg->name == mt_n;
                        }
                    );
                    if (it == arguments.end()) {
                        return {
                            error_obj.unplaced_arg(mt_n),
                            false,false
                        };
                    }
                }
            }

            for (auto& ar: arguments) {
                auto method = func->methods[ar->name];
                if (!method.is_any && !are_types_compatible(method.type,ar->type) ) {
                    return {
                        error_obj.unexpected_argument_type(ar->name,method.type,ar->type),
                        false,false
                    };
                }
            }
        }
    }
    
    for (auto& ar: arguments){
        auto arg_accept_r = ar->accept(ParentScope);
        if (!arg_accept_r.success) 
            return {arg_accept_r.Message,false,false};
    }

    return {"",true,true};
}


// Execute node
ReturnResult<Value> 
FunctionCallNode::exec(
    Scopes::Scope* ParentScope
) {
    ExternalFunInType args_list;
    for (auto& ar: arguments){
        auto arg_exec_r = ar->exec(ParentScope);
        if (!arg_exec_r.success) 
            return {arg_exec_r.Message,false,std::monostate()};

        args_list[ar->name] = ar->value;
        ar = nullptr;
    }
    arguments.clear();

    if (func->type == Scopes::SymbolTableTypes::FunctionsTypes::Extenal)
        return func->external_func(args_list);
    return func->external_func(args_list);
}
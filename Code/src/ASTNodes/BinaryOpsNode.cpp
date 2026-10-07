// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/BinaryOpsNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Binary operatins node classes
// ==================================================================

std::unordered_map<TokenType,ComparitonOpsTypes> ComparitonSymbols_ToOps = {
    {TokenType::EQUAL_EQUAL,ComparitonOpsTypes::EQUAL},
    {TokenType::NOT_EQUAL,ComparitonOpsTypes::NOT_EQUAL},
    {TokenType::LESS_THAN,ComparitonOpsTypes::LESS_THEN},
    {TokenType::GREATER_THAN,ComparitonOpsTypes::GREATER_THEN},
    {TokenType::LESS_EQUAL,ComparitonOpsTypes::LESS_THEN_OR_EQUAL},
    {TokenType::GREATER_EQUAL,ComparitonOpsTypes::GREATER_THEN_OR_EQUAL}
};

// ==================================================================
// Binary operatins node classes
// ==================================================================

// Constructure
BinOpsNode::BinOpsNode(
    OperationPartsList& parts_
): 
    Parts(std::move(parts_)) {}

// Get str to print
std::string 
BinOpsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Binary operation " << ":\n";

    for (auto& part: Parts) {
        for (int i=0;i<level+1;i++)
            ss << "|  ";

        if (part.op == TokenType::PLUS) ss << "plus+" << ":\n";
        else if (part.op == TokenType::MINUS) ss << "minus-" << ":\n";
        else if (part.op == TokenType::STAR) ss << "multiple*" << ":\n";
        else if (part.op == TokenType::SLASH) ss << "divide/" << ":\n";
        else ss << "First: " << "\n";



        ss << part.node->get_str(level+2);
    }

    return ss.str();
}

// get type of node
ASTNodesTypes 
BinOpsNode::NType() {
    return NT__BinOpsNode;
}

// Typess checking
ReturnResult<bool> 
BinOpsNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "float";
    for (auto& part: Parts) {
        auto accept_result = part.node->accept(ParentScope);
        if (!accept_result.success) {
            return {accept_result.Message,false,false};
        }
        if (part.node->return_type != "int" && part.node->return_type != "float") {
            return {Errors::TypeError(
                "int | float",
                part.node->return_type,
                part.token.line,
                part.token.column,
                "This operation does not support type else then int or float"
            ).msg,false,false};
        }
    }
    return {"",true,true};
}

// Do operation on a node
void
BinOpsNode::push_to_result(
    long double& result, 
    TokenType op, 
    long double input
) {
    if (op == TokenType::PLUS) {
        result += input;
    } else if (op == TokenType::MINUS) {
        result -= input;
    } else if (op == TokenType::STAR) {
        result *= input;
    } else if (op == TokenType::SLASH) {
        result /= input;
    }
}

// execute operation node
ReturnResult<Value> 
BinOpsNode::exec(
    Scopes::Scope* ParentScope
) { 
    long double result = 0;

    for (size_t i=0;i<Parts.size();i++) {
        auto& part = Parts[i];
        
        auto exec_result = part.node->exec(ParentScope);
        if (!exec_result.success) return {exec_result.Message,false,std::monostate{}};

        auto val = turn_value_to_float(exec_result.value);

        // Verifi that there is noot any devision oon zero
        if (part.op == TokenType::SLASH && val.value == 0) {
            Errors::RuntimeError error(
                part.token.line,
                part.token.column
            );
        
            return {error.division_by_zero(),false,0};
        }

        // Apply OP
        if (i == 0)
            result = val.value;
        else 
            push_to_result(result,part.op,val.value);
    }

    return {"",true,result};
}

// Get a new copy of class
ASTNode*
BinOpsNode::clone() {
    OperationPartsList new_parts;

    for (auto& p: Parts) {
        std::unique_ptr<ASTNode> value_node(p.node->clone());
        new_parts.push_back({
            p.op,
            p.token,
            std::move(value_node)
        });
    }

    return new BinOpsNode(new_parts);
};

// ==================================================================
// Comparition operatins node classes
// ==================================================================

// Constructure
CompOpsNode::CompOpsNode(
    OperationPart& FToken, 
    OperationPart& SToken,
    ComparitonOpsTypes T
): 
    first_part(std::move(FToken)),
    second_part(std::move(SToken)),
    type(T) {}

// Get str to print
std::string 
CompOpsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Comparition operation " << ":\n";

    std::string op_type = "EQUAL";

    if (type == ComparitonOpsTypes::NOT_EQUAL) op_type = "NOT_EQUAL";
    else if (type == ComparitonOpsTypes::LESS_THEN) op_type = "LESS_THEN";
    else if (type == ComparitonOpsTypes::GREATER_THEN) op_type = "GREATER_THEN";
    else if (type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL) op_type = "LESS_THEN_OR_EQUAL";
    else if (type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL) op_type = "GREATER_THEN_OR_EQUAL";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Type: " << op_type << "\n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "First part: \n" << first_part.node->get_str(level+2);

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Second part: \n" << second_part.node->get_str(level+2);

    return ss.str();
}

// get type of node
ASTNodesTypes 
CompOpsNode::NType() {
    return NT__CompOpsNode;
}

// Clone the class or get a new copy from them
ASTNode* 
CompOpsNode::clone() {

    std::unique_ptr<ASTNode> fpart_ptr(first_part.node->clone());
    std::unique_ptr<ASTNode> spart_ptr(second_part.node->clone());

    OperationPart FirstP = {
        TokenType::UNKNOWN,
        first_part.token,
        std::move(fpart_ptr)
    };
    OperationPart SecondP = {
        TokenType::UNKNOWN,
        second_part.token,
        std::move(spart_ptr)
    };

    return new CompOpsNode(FirstP,SecondP,type);
}

// type and value checking
ReturnResult<bool> 
CompOpsNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "bool";

    // Verifi parts
    auto fresult = first_part.node->accept(ParentScope);
    if (!fresult.success) return fresult;
    auto sresult = second_part.node->accept(ParentScope);
    if (!sresult.success) return sresult;

    // Verifi parts's types
    if (!are_types_compatible(first_part.node->return_type,second_part.node->return_type))  {
        return {
            fmt::format(
                "UncompatibleTypesError: can't compare `{}` with `{}` at line:{} ,column:{}",
                first_part.node->return_type, second_part.node->return_type, second_part.token.line, second_part.token.column
            ),
            false,false};
    }

    if (
        (
            type == ComparitonOpsTypes::LESS_THEN ||
            type == ComparitonOpsTypes::GREATER_THEN ||
            type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL ||
            type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL

        ) && !(
            (first_part.node->return_type == "int" && second_part.node->return_type == "float") ||
            (first_part.node->return_type == "float" && second_part.node->return_type == "int")

        ) && first_part.node->return_type != second_part.node->return_type
    ) {
        return {
            fmt::format(
                "UncompatibleTypesError: can't compare `{}` with `{}` at line:{} ,column:{} \n These types deos't sepport this compare operation",
                first_part.node->return_type, second_part.node->return_type, second_part.token.line, second_part.token.column
            ),
            false,false};
    }

    return {"",true,true};
}

// execute and get the result of compariton
ReturnResult<Value> 
CompOpsNode::exec(
    Scopes::Scope* ParentScope
) {

    // Execute parts
    auto fresult = first_part.node->exec(ParentScope);
    if (!fresult.success) return fresult;
    auto sresult = second_part.node->exec(ParentScope);
    if (!sresult.success) return sresult;

    // get return nodes
    auto FN_return_t = first_part.node->return_type;
    auto SN_return_t = second_part.node->return_type;

    // Delete parts nodes
    first_part.node = nullptr;
    second_part.node = nullptr;

    // Do comparition op ------
    Value cp_result;

    // Equal op (==)
    if (type == ComparitonOpsTypes::EQUAL) 
        cp_result = fresult.value == sresult.value;

    // Non-equal op (!=)
    else if(type == ComparitonOpsTypes::NOT_EQUAL)
        cp_result = fresult.value != sresult.value;


    if ((FN_return_t == "int" || FN_return_t == "float") && are_types_compatible(FN_return_t,SN_return_t)) {

        auto fv  = turn_value_to_float(fresult.value).value;
        auto sv  = turn_value_to_float(sresult.value).value;
    
        // Equal op (==)
        if (type == ComparitonOpsTypes::EQUAL) 
            cp_result = fv == sv;

        // Non-equal op (!=)
        else if(type == ComparitonOpsTypes::NOT_EQUAL)
            cp_result = fv != sv;

        // Less-then op (<)
        else if (type == ComparitonOpsTypes::LESS_THEN)
            cp_result = fv < sv;

        // GREATER-then op (>)
        else if (type == ComparitonOpsTypes::GREATER_THEN)
            cp_result = fv > sv;

        // Less-then or equal op (<=)
        else if (type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL)
            cp_result = fv <= sv;

        // GREATER-then or equal op (>=)
        else if (type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL)
            cp_result = fv >= sv;
        
    } else {
        // Equal op (==)
        if (type == ComparitonOpsTypes::EQUAL) 
            cp_result = fresult.value == sresult.value;

        // Non-equal op (!=)
        else if(type == ComparitonOpsTypes::NOT_EQUAL)
            cp_result = fresult.value != sresult.value;
    }

    // Return result -----
    return {"",true,cp_result};
}

// ==================================================================
// AND (||) and OR (||) Logical operation
// ==================================================================

// Constructor
LogicOpsNode::LogicOpsNode(
    OperationPartsList& parts
):Parts(std::move(parts)) {}
 
// Get str to print
std::string 
LogicOpsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Logical operation :\n";

    for (auto& part: Parts) {
        for (int i=0;i<level+1;i++)
            ss << "|  ";

        if (part.op == TokenType::LOGICAL_AND) ss << "and&&" << ":\n";
        else if (part.op == TokenType::LOGICAL_OR) ss << "OR||" << ":\n";
        else ss << "First: " << "\n";

        ss << part.node->get_str(level+2);
    }

    return ss.str();
}

// get type of node
ASTNodesTypes 
LogicOpsNode::NType() {
    return NT__LogicOpsNode;
}

// Get a new copy of class
ASTNode*
LogicOpsNode::clone() {
    OperationPartsList new_parts;

    for (auto& p: Parts) {
        std::unique_ptr<ASTNode> value_node(p.node->clone());
        new_parts.push_back({
            p.op,
            p.token,
            std::move(value_node)
        });
    }

    return new LogicOpsNode(new_parts);
};

// types and value checking 
ReturnResult<bool> 
LogicOpsNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "bool";

    // Verifi all parts
    for (auto& part: Parts) {
        // verifi part
        auto r = part.node->accept(ParentScope);
        if (!r.success) return r;

        // Verifi type
        if(part.node->return_type != "bool") {
            return {Errors::TypeError(
                "bool", part.node->return_type,
                part.token.line, part.token.column
            ).msg,false,false};
        }
    }

    return {"",true,true};
}

ReturnResult<Value> 
LogicOpsNode::exec(
    Scopes::Scope* ParentScope
) {

    // Get the first part node
    auto r = Parts[0].node->exec(ParentScope);
    if (!r.success) return r;

    // Get the first part node value and put it in result
    bool result = *(std::get_if<bool>(&r.value));

    // Get values of auther parts
    for (int i=1;i<Parts.size(); i++) {

        auto& part = Parts[i];

        // Execute part
        r = part.node->exec(ParentScope);
        if (!r.success) return r;

        // Get part result
        bool part_r = *(std::get_if<bool>(&r.value));

        // Apply to main result
        if (part.op == TokenType::LOGICAL_AND) {
            result = result && part_r;
        } else if (part.op == TokenType::LOGICAL_OR){
            result = result || part_r;
        }
    }

    // Return result
    return {"",true,result};
}
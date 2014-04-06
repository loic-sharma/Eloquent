#include "virtual_machine.h"
#include "ast.h"

#include <iostream>
#include <stack>

#include "value.h"

void VirtualMachine::execute(Node *AST) {
	Symbols *symbols = new Symbols;
    state = NormalState;

	evaluate(AST, symbols);

	delete symbols;
}

Value *VirtualMachine::evaluate(Node *node, Symbols *symbols) {
	if (node == nullptr) return nullptr;

    if (symbols == nullptr) {
        std::cerr << "Symbols table cannot be null" << std::endl;
        exit(1);
    }

    switch (node->type) {
        case Node::Compound: {
            Value *result = evaluate(node->left, symbols);
            
            if (state == ReturnState) {
                return result;
            }
            else {
                return evaluate(node->right, symbols);
            }

            break;
        }

        case Node::Print: {
            Value *expression = evaluate(node->left, symbols);

            if (expression == nullptr) {
                std::cerr << "Nothing to print." << std::endl;
                exit(1);
            }

            std::cout << expression->to_string() << std::endl;

            return nullptr;
            break;
        }

        case Node::Function: {
            if(node->left == nullptr or node->left->left == nullptr) {
                std::cerr << "Function has invalid prototype." << std::endl;
                exit(1);
            }
            else {
                std::vector<std::string> parameters;

                if (node->left->right) {
                    std::stack<Node *> nodes;
 
                    nodes.push(node->left->right);

                    while (nodes.empty() == false) {
                        Node *current = nodes.top();

                        if (current->type == Node::Separator) {
                            nodes.pop();

                            if (current->right) nodes.push(current->right);
                            if (current->left) nodes.push(current->left);
                        }
                        else {
                            parameters.push_back(current->value->to_string());

                            nodes.pop();
                        }
                    }
                }

                std::string function_name = node->left->left->value->to_string();

                functions[function_name] = {
                    function_name,
                    parameters,
                    node->right
                };
            }

            return nullptr;
            break;
        }

        case Node::Conditional: {
            if (node->left == nullptr or node->left->type != Node::Branch) {
                std::cerr << "Conditional must have a branch." << std::endl;
                exit(1);
            }

            if (node->left->left == nullptr) {
                std::cerr << "Conditional must have a condition." << std::endl;
                exit(1);
            }


            Value *condition = evaluate(node->left->left, symbols);

            if (condition->to_bool()) {
                return evaluate(node->left->right, symbols);
            }
            else {
                return evaluate(node->right, symbols);
            }

            break;
        }

        case Node::While: {
            bool done = false;

            while (done == false) {
                Value *condition = evaluate(node->left, symbols);

                if (condition == nullptr) {
                    std::cerr << "While condition cannot be null." << std::endl;
                    exit(1);
                }

                if (condition->to_bool()) {
                    Value *result = evaluate(node->right, symbols);

                    if (state == ReturnState) {
                        return result;
                    }
                }
                else {
                    done = true;
                }
            }

            return nullptr;
            break;
        }

        case Node::Equals: {
            if (node->left == nullptr or node->right == nullptr) {
                std::cerr << "Must have two values to do a comparison." << std::endl;
                exit(1);
            }

            Value *left = evaluate(node->left, symbols);
            Value *right = evaluate(node->right, symbols);
            bool result = false;

            switch (left->get_type()) {
                case BooleanTypeValue:
                    result = left->to_bool() == right->to_bool();
                    break;
                case IntegerTypeValue:
                    result = left->to_integer() == right->to_integer();
                    break;
                case DoubleTypeValue:
                    result = left->to_double() == right->to_double();
                    break;
                case StringTypeValue:
                    result = left->to_string() == right->to_string();
                    break;

                case UnknownTypeValue:
                case NullTypeValue:
                    result = left->get_type() == right->get_type();
                    break;

                default:
                case ArrayTypeValue:
                    std::cerr << "Attempting to compare uncomparable types." << std::endl;
                    exit(1);
                    break;
            }

            return new Value(result);
            break;
        }

        case Node::Call: {
            if (node->left == nullptr) {
                std::cerr << "Function cannot have an empty identifier." << std::endl;
                exit(1);
            }

            auto it = functions.find(node->left->value->to_string());

            if (it == functions.end()) {
                std::cerr << "Invalid function '" << node->left->value->to_string() << "'" << std::endl;
                exit(1);
            }

            Symbols *parameters = new Symbols;
            unsigned parameter_count = 0;

            if (node->right) {
                std::stack<Node *> nodes;

                nodes.push(node->right);

                while (nodes.empty() == false) {
                    Node *current = nodes.top();

                    if (current->type == Node::Separator) {
                        nodes.pop();

                        if (current->right) nodes.push(current->right);
                        if (current->left) nodes.push(current->left);
                    }
                    else {
                        if (it->second.parameters.size() <= parameter_count) {
                            std::cerr << "Too many parameters." << std::endl;
                            exit(1);
                        }

                        std::string name(it->second.parameters[parameter_count++]);

                        Value *value = evaluate(current, symbols);

                        if (value == nullptr) {
                            std::cerr << "Parameter cannot be null." << std::endl;
                            exit(1);
                        }

                        (*parameters)[name] = value;

                        nodes.pop();
                    }
                }
            }

            Value *result = evaluate(it->second.content, parameters);

            state = NormalState;

            delete parameters;

            return result;
            break;
        }

        case Node::Return: {
            Value *result = evaluate(node->left, symbols);

            if (result == nullptr) {
                std::cerr << "Must return a value." << std::endl;
                exit(1);
            }

            state = ReturnState;

            return result;
            break;
        }

        case Node::Assign: {
            if (node->left and node->right) {
                Value *value = evaluate(node->right, symbols);

                if (value) {
                    (*symbols)[node->left->value->to_string()] = value;
                }
                else {
                    std::cerr << "Value is a null pointer." << std::endl;
                    exit(1);
                }

                return nullptr;
            }
            else {
                std::cerr << "Missing left or right child for assignment." << std::endl;
                exit(1);
            }

            break;
        }

        case Node::Add:
        case Node::Sub:
        case Node::Mult:
        case Node::Div:
        case Node::Mod: {
            Value *left = evaluate(node->left, symbols);

            if (left == nullptr) {
                std::cerr << "Left side cannot be null." << std::endl;
                exit(1);
            }

            if ((left->get_type() & (IntegerTypeValue | DoubleTypeValue)) == 0) {
                std::cout << "Type: " << left->get_type() << std::endl;
                std::cerr << "Cannot perform arithmetic on value '" << left->to_string() << "'." << std::endl;
                exit(1);         
            }

            Value *right = evaluate(node->right, symbols);

            if (right == nullptr) {
                std::cerr << "Left side cannot be null." << std::endl;
                exit(1);
            }
            
            if ((right->get_type() & (IntegerTypeValue | DoubleTypeValue)) == 0) {
                std::cerr << "Cannot perform arithmetic on value '" << left->to_string() << "'." << std::endl;
                exit(1);         
            }

            // If one of these values is a double we will need to return a double.
            if (left->get_type() & DoubleTypeValue or right->get_type() & DoubleTypeValue) {
                double lval = left->to_double();
                double rval = right->to_double();
                double result;

                switch (node->type) {
                    case Node::Add:
                        result = lval + rval;
                        break;

                    case Node::Sub:
                        result = lval - rval;
                        break;

                    case Node::Mult:
                        result = lval * rval;
                        break;

                    case Node::Div:
                        result = lval / rval;
                        break;

                    case Node::Mod:
                        std::cerr << "Cannot calculate mod of double." << std::endl;
                        exit(1);
                        break;

                    default:
                        std::cerr << "Invalid type: " << node->type << std::endl;
                        exit(1);
                        break;
                }
            }
            else {
                long lval = left->to_integer();
                long rval = right->to_integer();
                long result;

                switch (node->type) {
                    case Node::Add:
                        result = lval + rval;
                        break;

                    case Node::Sub:
                        result = lval - rval;
                        break;

                    case Node::Mult:
                        result = lval * rval;
                        break;

                    case Node::Div:
                        result = lval / rval;
                        break;

                    case Node::Mod:
                        result = lval % rval;
                        break;

                    default:
                        std::cerr << "Invalid type: " << node->type << std::endl;
                        exit(1);
                        break;
                }

                return new Value(result);
            }

            return nullptr;
        }

        case Node::Identifier: {
            if (node->value == nullptr) {
                std::cerr << "Identifier cannot be null." << std::endl;
                exit(1);
            }
            else if ((node->value->get_type() & StringTypeValue) == 0) {
                std::cerr << "Identifier must be a string." << std::endl;
                exit(1);
            }

            std::string identifier = node->value->to_string();

            if (symbols->find(identifier) != symbols->end()) {
                return (*symbols)[identifier];
            }
            else {
                std::cerr << "Variable '" << identifier << "' does not exist." << std::endl;
                exit(1);               
            }

            break;
        }

        case Node::Integer:
        case Node::Double:
        case Node::String:
        case Node::Boolean:
            return node->value;
            break;

        default:
            return nullptr;
            break;
    }
}
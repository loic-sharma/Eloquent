#include "virtual_machine.h"
#include "ast.h"

#include <iostream>
#include <stack>

void VirtualMachine::execute(Node *AST) {
	Symbols *symbols = new Symbols;

	evaluate(AST, symbols);

	delete symbols;
}

int *VirtualMachine::evaluate(Node *node, Symbols *symbols) {
	if (node == nullptr) return nullptr;

    if (symbols == nullptr) {
        std::cerr << "Symbols table cannot be null" << std::endl;
        exit(1);
    }

    switch (node->type) {
        case Node::Compound: {
            int *left_ptr = evaluate(node->left, symbols);
            int *right_ptr = evaluate(node->right, symbols);

            delete left_ptr;

            return right_ptr;
            break;
        }

        case Node::Print: {
            int *expression = evaluate(node->left, symbols);

            if (expression == nullptr) {
                std::cerr << "Nothing to print." << std::endl;
                exit(1);
            }

            std::cout << *expression << std::endl;
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
                            parameters.push_back(current->value);

                            nodes.pop();
                        }
                    }
                }

                std::string function_name(node->left->left->value);

                functions[function_name] = {
                    function_name,
                    parameters,
                    node->right
                };
            }

            return nullptr;
            break;
        }

        case Node::Call: {
            auto it = functions.find(node->left->value);

            if (it == functions.end()) {
                std::cerr << "Invalid function '" << node->left->value << "'" << std::endl;
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
                        std::string name(it->second.parameters[parameter_count++]);

                        (*parameters)[name] = atoi(current->value);

                        nodes.pop();
                    }
                }
            }

            int *result = evaluate(it->second.content, parameters);

            delete parameters;

            return result;
            break;
        }

        case Node::Return:
            return evaluate(node->left, symbols);
            break;

        case Node::Assign: {
            if (node->left and node->right) {
                int *value = evaluate(node->right, symbols);

                if (value) {
                    (*symbols)[node->left->value] = *value;
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
        }

        case Node::Add:
        case Node::Sub:
        case Node::Mult:
        case Node::Div:
        case Node::Mod: {
            int *left_ptr = evaluate(node->left, symbols);

            if (left_ptr == nullptr) {
                std::cerr << "Left side cannot be null." << std::endl;
                exit(1);
            }

            int *right_ptr = evaluate(node->right, symbols);

            if (right_ptr == nullptr) {
                std::cerr << "Left side cannot be null." << std::endl;
                exit(1);
            }
            
            int left = *left_ptr, right = *right_ptr;

            delete left_ptr;
            delete right_ptr;

            switch (node->type) {
                case Node::Add:
                    return new int(left + right);
                    break;

                case Node::Sub:
                    return new int(left - right);
                    break;

                case Node::Mult:
                    return new int(left * right);
                    break;

                case Node::Div:
                    return new int(left / right);
                    break;

                case Node::Mod:
                    return new int(left % right);
                    break;

                default:
                    std::cerr << "Invalid type: " << node->type << std::endl;
                    exit(1);
                    break;
            }
        }

        case Node::Identifier:
            if (node->value == nullptr) {
                std::cerr << "Identifier cannot be null." << std::endl;
                exit(1);
            }

            if (node->value and (*node->value >= '0' and *node->value <= '9')) {
                return new int(atoi(node->value));
            }
            else {
                if (symbols->find(node->value) != symbols->end()) {
                    return new int((*symbols)[node->value]);
                }
                else {
                    std::cerr << "Variable '" << node->value << "' does not exist." << std::endl;
                    exit(1);
                }
            }
            break;

        default:
            return nullptr;
            break;
    }
}
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

    std::string value(node->value);

    if (value == "__print") {
        int *expression = evaluate(node->left, symbols);

        if (expression == nullptr) {
            std::cerr << "Nothing to print." << std::endl;
            exit(1);
        }

        std::cout << *expression << std::endl;
        return nullptr;
    }
    else if (value == "__function") {
        if(node->left == nullptr or node->left->left == nullptr) {
            std::cerr << "Function has invalid prototype" << std::endl;
            exit(1);
        }
        else {
            std::vector<std::string> parameters;
            std::stack<Node *> nodes;

            nodes.push(node->left->right);

            while (nodes.empty() == false) {
                Node *current = nodes.top();

                if (current->value[0] == ',') {
                    nodes.pop();

                    if (current->right) nodes.push(current->right);
                    if (current->left) nodes.push(current->left);
                }
                else {
                    parameters.push_back(current->value);

                    nodes.pop();
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
    }
    else if (value == "__call") {
        auto it = functions.find(node->left->value);

        if (it == functions.end()) {
            std::cerr << "Invalid function '" << node->left->value << "'" << std::endl;
            exit(1);
        }

        Symbols *parameters = new Symbols;
        unsigned parameter_count = 0;

        std::stack<Node *> nodes;

        nodes.push(node->right);

        while (nodes.empty() == false) {
            Node *current = nodes.top();

            if (current->value[0] == ',') {
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

        int *result = evaluate(it->second.content, parameters);

        delete parameters;

        return result;
    }
    else if(value == "__return") {
        return evaluate(node->left, symbols);
    }
    else if (value == "=") {
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
            std::cerr << "No left or right child for assignment." << std::endl;
            exit(1);
        }
    }
    else {
        int left, right;
        int *left_ptr = evaluate(node->left, symbols);
        int *right_ptr = evaluate(node->right, symbols);

        if (left_ptr) {
            left = *left_ptr;

            delete left_ptr;
        }

        if (right_ptr) {
            right = *right_ptr;

            delete right_ptr;
        }

        if (value == "*") {
            return new int(left * right);
        }
        else if (value == "/") {
            return new int(left / right);
        }
        else if (value == "+") {
            return new int(left + right);
        }
        else if (value == "-") {
            return new int(left - right);
        }
        else if (value == "%") {
            return new int(left % right);   
        }
        else if (value == "__compound") {
            int *output = (right_ptr) ? new int(right) : nullptr;

            return output;
        }
        else {
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
        }
    }
}
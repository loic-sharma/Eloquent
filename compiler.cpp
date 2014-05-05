#include "compiler.h"
#include "value.h"
#include "ast.h"

#include <cassert>
#include <iostream>

void Program::print() {
	static std::string names[] = {
		"JumpType",
		"PrintType",
		"CallType",
		"ReturnType",
		"AssignType",
		"TrueJumpType",
		"FalseJumpType",
		"AndType",
		"OrType",
		"EqualsType",
		"NEqualsType",
		"IncrementType",
		"DecrementType",
		"AddType",
		"SubType",
		"MultType",
		"DivType",
		"ModType",
		"IdentifierType",
		"ValueType",
		"EndType"
	};

	for (auto i = instructions->begin(); i != instructions->end(); ++i) {
		std::cout << names[i->type];

		if (i->type == Instruction::IdentifierType or i->type == Instruction::ValueType or i->type == Instruction::AssignType) {
			std::cout << " (" << i->value->to_string() << ')';
		}
		else if (i->type == Instruction::JumpType or i->type == Instruction::TrueJumpType or i->type == Instruction::FalseJumpType) {
			std::cout << " (" << i->flags << ')';
		}

		std::cout << '\n';
	}
}

Program *Compiler::compile(Node *ast) {
	Program *program = new Program;

	if (ast != nullptr) {
		program->instructions = compile_node(program, ast);
	}
	else {
		program->instructions = new Instructions;
	}

	program->instructions->push_back({
		nullptr,
		Instruction::EndType,
		0
	});

	return program;
}

Instructions *Compiler::compile_node(Program *program, Node *node) {
	if (node == nullptr) return nullptr;

	assert(program);

	Instructions *compiled = new Instructions;

	switch (node->type) {
		case Node::Compound: {
			Instructions *left = compile_node(program, node->left);
			Instructions *right = compile_node(program, node->right);

			assert(left);
			assert(right);

			compiled->insert(compiled->end(), left->begin(), left->end());
			compiled->insert(compiled->end(), right->begin(), right->end());

			break;
		}

		case Node::Function: {
			assert(node->left);
			assert(node->left->left);

			std::string name = node->left->left->value->to_string();

			program->functions[name] = compile_node(program, node->right);

			break;
		}

		case Node::Conditional: {
			assert(node->left and node->left->type == Node::Branch);
			assert(node->left->left);

			// Let's compile the three parts to the conditional: it's condition's expression,
			// the block to execute if the condition is true, and the block to execute if the
			// condition is false.
			Instructions *expression = compile_node(program, node->left->left);
			Instructions *then_block = compile_node(program, node->left->right);
			Instructions *else_block = compile_node(program, node->right);

			assert(expression);

			// Evaluate the condition's expression.
			compiled->insert(compiled->end(), expression->begin(), expression->end());

			// Insert the conditional's then_block block, if it has one.
			if (then_block) {
				// If the condition is false we will want to skip the conditional's
				// then block. We also skip the instruction that jumps from the end of
				// the main block to after the end of the else block.
				compiled->push_back({
					nullptr,
					Instruction::FalseJumpType,
					(long) then_block->size()
				});

				compiled->insert(compiled->end(), then_block->begin(), then_block->end());				
			}

			if (else_block) {
				// Insert a jump instruction at the end of the main block to
				// skip the else's block.
				compiled->push_back({
					nullptr,
					Instruction::JumpType,
					(long) else_block->size()
				});

				compiled->insert(compiled->end(), else_block->begin(), else_block->end());

				delete else_block;
			}

			delete expression;
			delete then_block;
			break;
		}

		case Node::Loop: {
			Instructions *condition = compile_node(program, node->left);
			Instructions *block = compile_node(program, node->right);

			assert(condition);
			assert(block);

			compiled->insert(compiled->end(), condition->begin(), condition->end());

			// Skip the rest of the loop if the condition evaluates to false.
			compiled->push_back({
				nullptr,
				Instruction::FalseJumpType,
				(long) block->size() + 1
			});

			compiled->insert(compiled->end(), block->begin(), block->end());

			// Jump back to the very beginning of the loop and continue on loopin'.
			compiled->push_back({
				nullptr,
				Instruction::JumpType,
				-1 * (long) (condition->size() + block->size() + 1)
			});

			delete condition;
			delete block;
			break;
		}

		case Node::Print: {
			Instructions *left = compile_node(program, node->left);

			assert(left);

			compiled->insert(compiled->end(), left->begin(), left->end());
			compiled->push_back({
				nullptr,
				Instruction::PrintType,
				0
			});

			break;
		}

		case Node::Call:
			compiled->push_back({
				nullptr,
				Instruction::CallType,
				0
			});

			break;

		case Node::Return: {
			Instructions *left = compile_node(program, node->left);

			if (left != nullptr) {
				compiled->insert(compiled->end(), left->begin(), left->end());
			}

			compiled->push_back({
				nullptr,
				Instruction::ReturnType,
				0
			});

			break;
		}

		case Node::Assign: {
			assert(node->left->type == Node::Identifier);

			Instructions *value = compile_node(program, node->right);

			assert(value);

			compiled->insert(compiled->end(), value->begin(), value->end());

			compiled->push_back({
				node->left->value,
				Instruction::AssignType,
				0
			});

			break;
		}

		case Node::Or:
		case Node::And: {
			Instructions *left = compile_node(program, node->left);
			Instructions *right = compile_node(program, node->right);

			assert(left);
			assert(right);

			compiled->insert(compiled->end(), left->begin(), left->end());

			if (node->type == Node::And) {
				// Handle short circuiting. If the left hand side of the expression 
				// is false, the entire expression is false. Thus, we will jump to the end
				// of the and expression. We have to jump the right hand side as well as
				// the actual and instruction.
				compiled->push_back({
					nullptr,
					Instruction::FalseJumpType,
					(long) right->size() + 1,
				});

				compiled->insert(compiled->end(), right->begin(), right->end());
				compiled->push_back({
					nullptr,
					Instruction::AndType,
					0
				});
			}
			else {
				// Handle short circuiting. If the left hand side of the expression
				// is true, the entire expression is true. Thus, we will jump to the
				// end of the or expression. We ahve to jump the right hand side as
				// well as the actual or expression.
				compiled->push_back({
					nullptr,
					Instruction::TrueJumpType,
					(long) right->size() + 1,
				});

				compiled->insert(compiled->end(), right->begin(), right->end());
				compiled->push_back({
					nullptr,
					Instruction::OrType,
					0
				});
			}

			delete left;
			delete right;
			break;
		}

		case Node::Equals:
    	case Node::NEquals: {
    		Instructions *left = compile_node(program, node->left);
			Instructions *right = compile_node(program, node->right);

			assert(left);
			assert(right);
			
			compiled->insert(compiled->end(), left->begin(), left->end());
			compiled->insert(compiled->end(), right->begin(), right->end());

			compiled->push_back({
				nullptr,
				(node->type == Node::Equals) ? Instruction::EqualsType : Instruction::NEqualsType,
				0
			});

			delete left;
			delete right;
			break;
		}

		case Node::Increment:
		case Node::Decrement: {
			Instructions *expression = compile_node(program, node->left);

			assert(expression);

			compiled->insert(compiled->end(), expression->begin(), expression->end());
			compiled->push_back({
				nullptr,
				(node->type == Node::Increment) ? Instruction::IncrementType : Instruction::DecrementType,
				0
			});

			delete expression;
			break;
		}

		case Node::Add:
		case Node::Sub:
		case Node::Mult:
		case Node::Div:
		case Node::Mod: {
			Instructions *left = compile_node(program, node->left);
			Instructions *right = compile_node(program, node->right);

			assert(left);
			assert(right);
			
			compiled->insert(compiled->end(), left->begin(), left->end());
			compiled->insert(compiled->end(), right->begin(), right->end());

			Instruction::Type type;

			if (node->type == Node::Add) type = Instruction::AddType;
			else if (node->type == Node::Sub) type = Instruction::SubType;
			else if (node->type == Node::Mult) type = Instruction::MultType;
			else if (node->type == Node::Div) type = Instruction::DivType;
			else if (node->type == Node::Mod) type = Instruction::ModType;
			else assert (false);
			
			compiled->push_back({
				nullptr,
				type,
				0
			});

			delete left;
			delete right;
			break;
		}

		case Node::Identifier: {
			compiled->push_back({
				node->value,
				Instruction::IdentifierType,
				0
			});

			break;
		}

		case Node::Integer:
		case Node::Double:
		case Node::String:
		case Node::Boolean: {
			compiled->push_back({
				node->value,
				Instruction::ValueType,
				0
			});

			break;
		}

		default:
			assert(false);
			break;
	}

	return compiled;
}

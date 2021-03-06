#ifndef __Eloquent_ast__
#define __Eloquent_ast__

class Value;

class Node {
public:
	enum Type {
		Return, Assign,
		Add, Sub, Mult,
		Div, Mod,
		Increment, Decrement,
		Function, Print,
		Integer, Double, String,
		Boolean, Identifier,

		If, Equals, NEquals,
		And, Or,

		Compound, Prototype, Conditional,
		Branch, Call, Separator,

		Loop,
	};

	Node(const Type t, Node *l = nullptr, Node *r = nullptr) : type(t), value(nullptr), left(l), right(r) {};
	Node(const Type t, Value *v, Node *l = nullptr, Node *r = nullptr) : type(t), value(v), left(l), right(r) {};

	Type type;
	Value *value;

	Node *left;
	Node *right;
};

#endif
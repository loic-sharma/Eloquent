#ifndef __Eloquent_ast__
#define __Eloquent_ast__

class Value;

class Node {
public:
	enum Type {
		Return, Assign, Equals,
		Add, Sub, Mult,
		Div, Mod, Function,
		If, Print, Integer,
		Double, String, Boolean,
		Identifier,

		Compound, Prototype, Conditional,
		Branch, Call, Separator,
	};

	Node(const Type t, Node *l = nullptr, Node *r = nullptr) : type(t), value(nullptr), left(l), right(r) {};
	Node(const Type t, Value *v, Node *l = nullptr, Node *r = nullptr) : type(t), value(v), left(l), right(r) {};

	Type type;
	Value *value;

	Node *left;
	Node *right;
};

#endif
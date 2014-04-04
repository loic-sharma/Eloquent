#ifndef __Eloquent_ast__
#define __Eloquent_ast__

class Node {
public:
	enum Type {
		Return, Assign, Equals,
		Add, Sub, Mult,
		Div, Mod, Function,
		If, Print, Integer,
		Double, String, Identifier,

		Compound, Prototype, Conditional,
		Call, Separator,
	};

	Node(const Type t, Node *l = nullptr, Node *r = nullptr) : type(t), value(nullptr), left(l), right(r) {};
	Node(const char *v, Node *l = nullptr, Node *r = nullptr) : type(Identifier), value(v), left(l), right(r) {};

	Type type;
	const char *value;

	Node *left;
	Node *right;
};

#endif
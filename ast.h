#ifndef __Eloquent_ast__
#define __Eloquent_ast__

class Node {
public:
	Node(const char *v, Node *l = nullptr, Node *r = nullptr) : value(v), left(l), right(r) {}

	const char *value;
	Node *left;
	Node *right;
};

class CompoundNode : public Node {
public:
	CompoundNode(Node *l, Node *r) : Node("__compound", l, r) {}
};

#endif

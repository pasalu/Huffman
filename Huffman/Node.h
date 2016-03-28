#ifndef NODE_H
#define NODE_H

class Node
{
public:
	char character;
	unsigned int frequency;
	Node* left;
	Node* right;

	Node(char character, unsigned int frequency);
	bool isLeaf();
};

struct NodeCompare
{
	bool operator()(const Node& lhs, const Node& rhs) const;
};

#endif NODE_H
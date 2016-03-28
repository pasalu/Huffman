#include "Node.h"

/**
 * Constructor for a new Node.
 *
 * @param character The character to be stored.
 * @param frequency The number of times charcter appears.
 */
Node::Node(char character, unsigned int frequency)
	:character(character), frequency(frequency), left(nullptr), right(nullptr)
{
}

/**
 * Checks to see if the node has any children
 *
 * @return True if it has a child, false otherwise.
 */
bool Node::isLeaf()
{
	//Huffman trees will have left child, or no children at all.
	return this->left == nullptr;
}

/**
 * Comparison operator for multiset. Nodes should be ordered based on frequency.
 *
 * @param node The other node to be compared.
 * @return true if this node is less than the other node, false otherwise.
 */
bool NodeCompare::operator()(const Node& lhs, const Node& rhs) const
{
	return lhs.frequency < rhs.frequency;
}
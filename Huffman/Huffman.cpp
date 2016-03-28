#include "Huffman.h"

/*
 * Reads characters from a file and returns the contents
 * 
 * @param fileName The name of the file to be read
 * @return The contents of the file read as bytes, and the number of characters in the file.
 */
std::pair<char*, std::size_t> getCharactersFromFile(char* fileName)
{
	std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);

	if(!file.is_open())
	{
		std::cerr << "Could not open: " << fileName << " for reading" << std::endl;
		exit(1);
	}

	//Since we used ios::ate to position the file pointer at the end, we can use
	//tellg to get the size (in bytes) of the file.
	std::ifstream::pos_type size = file.tellg();
	std::size_t numberOfCharacters = (size_t) size.seekpos();
	char* characters = new char[numberOfCharacters];

	//Go back to the begining, read all the characters, and close the file.
	file.seekg(0, std::ios::beg);
	file.read(characters, size);
	file.close();

	return std::make_pair(characters, numberOfCharacters);
}

/**
 * Gets how frequently characters appear.
 *
 * @param characters The characters and how many there are.
 * @return A map of character frequency pairs.
 */
std::map<char, unsigned int> getCharactersAndFrequencies(const std::pair<char*, std::size_t>& characters)
{
	std::map<char, unsigned int> charactersAndFrequencies;
	char character;

	for(unsigned int i = 0; i < characters.second; i++)
	{
		character = characters.first[i];
		//If character didn't exist, it would be initialized to 0, then incremented.
		charactersAndFrequencies[character]++;
	}

	return charactersAndFrequencies;
}

/**
 * Removes and returns the node with the smallest frequncy.
 * 
 * @param nodes The multiset of nodes sorted in ascending order.
 * @return The node with the smallest frequency
 */
Node* removeLowestFrequency(std::multiset<Node, NodeCompare>* nodes)
{
	std::multiset<Node, NodeCompare>::iterator i = nodes->begin();
	Node* node = new Node(i->character, i->frequency);
	node->left = i->left;
	node->right = i->right;
	nodes->erase(i);

	return node;
}

/**
 * Constructs a huffman tree.
 * 
 * @param charactersAndFrequencies A map of characters and their frequency.
 * @return The root of the Huffman tree.
 */
Node* constructHuffmanTree(const std::map<char, unsigned int>& charactersAndFrequencies)
{
	std::multiset<Node, NodeCompare> nodes;

	//Initialize nodes to the content of charactersAndFrequencies
	for(std::map<char, unsigned int>::const_iterator i = charactersAndFrequencies.cbegin(); i != charactersAndFrequencies.cend(); i++)
	{
		nodes.insert(Node(i->first, i->second));
	}

	while(nodes.size() > 1)
	{
		Node* parent = new Node('\0', 0);
		parent->left = removeLowestFrequency(&nodes);
		parent->right = removeLowestFrequency(&nodes);
		parent->frequency = parent->left->frequency + parent->right->frequency;
		nodes.insert(*parent);
	}

	return removeLowestFrequency(&nodes);
}

/**
 * Gets the bits for leaf character in the Huffman tree. Adapted from recurs_print_codes in Algorithms by Richard Johnsonbaugh.
 * 
 * @param root The root of the Huffman tree.
 * @param s A variable to store the intermediate value of the bits.
 * @param A map of characters and their corresponding bits. The result will be stored here.
 */
void getCodes(Node* root, std::string s, std::map<char, std::string>* codes)
{
	if(root->isLeaf())
	{
		(*codes)[root->character] = s;
	}
	else
	{
		//0 means left branch, 1 means right.
		getCodes(root->left, s + "0", codes);
		getCodes(root->right, s + "1", codes);
	}
}

/**
 * Pads the end of a string with 0's until it's length is 8.
 *
 * @param s The string to pad with 0's.
 * @return The string padded with 0's.
 */
std::string padWithZeros(std::string s)
{
	while(s.length() < 8)
	{
		s.append("0");
	}

	return s;
}

/**
 * Writes the compressed representation of characters into a file.
 *
 * @param file The file to write the compressed characters to.
 * @param codes A map of characters and their compressed represenation.
 * @param characters The characters as they were read from a file.
 */
void writeCodesToFile(std::ofstream& file, const std::map<char, std::string>& codes, const std::pair<char*, std::size_t>& characters)
{
	const unsigned int BYTE_LENGTH = 8;
	std::string codeString;
	std::string firstEightCharacters;
	char inputCharacter = '\0';
	bool lastIterationOfLoop = false;
	
	for(unsigned int i = 0; i < characters.second; i++)
	{
		inputCharacter = characters.first[i];
		codeString.append(codes.at(inputCharacter));

		if(codeString.length() >= BYTE_LENGTH)
		{
			firstEightCharacters = codeString.substr(0, BYTE_LENGTH);
			codeString = codeString.substr(BYTE_LENGTH);
		}

		lastIterationOfLoop = (i == characters.second - 1);

		//We are in the last iteration of the loop, or we have 8 characters.
		if( (lastIterationOfLoop) || (firstEightCharacters != "") )
		{
			if(lastIterationOfLoop && firstEightCharacters == "")
			{
				firstEightCharacters = codeString;
				codeString = "";
			}

			//Since the last byte might be padded with 0's, the last bits will be invalid. We write the number
			//of valid bits in the second to last byte.
			char numberOfValidBits = '\0';
			
			if(lastIterationOfLoop)
			{
				if(codeString == "")
				{
					file << (char)firstEightCharacters.length();
				}
				else
				{
					numberOfValidBits = (char)codeString.length();
				}
			}

			firstEightCharacters = padWithZeros(firstEightCharacters);
			std::bitset<BYTE_LENGTH> aByte(firstEightCharacters);
			file << (char)aByte.to_ulong();

			if(lastIterationOfLoop && codeString != "")
			{
				file << numberOfValidBits;
				codeString = padWithZeros(codeString);
				std::bitset<BYTE_LENGTH> codeBytes(codeString);
				file << (char)codeBytes.to_ulong();
			}

			firstEightCharacters = "";
		}
	}
}

/**
 * Stores the Huffman tree in a file.
 * 
 * @param file The file to store the tree in.
 * @param root The root of the Huffman tree.
 */
void writeTreeToFile(std::ofstream& file, Node* root)
{
	writeTreeRecursively(file, root);
	
	//An internal node with a non-null character will mark the end of the tree in the file.
	const char internalNodeMarker = 0;
	const char nonNullCharacter = 1;
	file << internalNodeMarker << nonNullCharacter;
	file.close();
}


/**
 * Recurses through the tree and writes the nodes to the file. In depth first order.
 *
 * @param file An opened file where the tree will be stored.
 * @param root The root of the Huffman tree.
 */
void writeTreeRecursively(std::ofstream& file, Node* root)
{
	const char internalNodeMarker = 0;
	const char leafNodeMarker = 1;

	//Perform Post-Order traversal of the tree.
	if(root != nullptr)
	{
		writeTreeRecursively(file, root->left);
		writeTreeRecursively(file, root->right);
		//Leaf nodes will be of the form 1Character, internal nodes will be 0Character
		if(root->isLeaf())
		{
			file << leafNodeMarker;
		}
		else
		{
			file << internalNodeMarker;
		}

		file << root->character;
	}
}

/**
 * Deletes the nodes in a Huffman tree.
 *
 * @param root The root of the Huffman tree.
 */
void deleteNodes(Node* root)
{
	//Post-Order traversal of the tree to ensure we delete child nodes before their parents.
	if(root != nullptr)
	{
		deleteNodes(root->left);
		deleteNodes(root->right);
		delete root;
	}
}

/**
 * Huffman encodes the contents of a file. The output will be in [filename].enc
 *
 * @param filename The name of the file whose contents will be encoded.
 */
void huffmanEncode(char* filename)
{
	std::pair<char*, size_t> characters = getCharactersFromFile(filename);
	std::map<char, unsigned int> charactersAndFrequencies = getCharactersAndFrequencies(characters);
	Node* root = constructHuffmanTree(charactersAndFrequencies);
	std::map<char, std::string> codes;
	getCodes(root, "", &codes);

	std::string outputFilename(filename);
	outputFilename.append(".enc");
	std::ofstream file(outputFilename, std::ios::binary | std::ios::app);

	if(!file.is_open())
	{
		std::cerr << "Could not open: " << outputFilename << " for writing" << std::endl;
		exit(1);
	}

	writeTreeToFile(file, root);
	writeCodesToFile(file, codes, characters);
	file.close();

	delete[] characters.first;
	deleteNodes(root);
}

/**
 * Reads the Huffman tree from the contents of a file.
 *
 * @param fileContents The contents of a file which contains the tree.
 * @return The root of the Huffman tree, and the location in fileContents where the tree ends.
 */
std::pair<Node*, size_t> getTree(std::pair<char*, std::size_t> fileContents)
{
	const char internalNodeMarker = 0;
	const char leafNodeMarker = 1;
	const char nonNullCharacter = 1;
	char marker = ' ';
	char character = ' ';
	unsigned int i = 0;
	std::stack<Node*> nodes;

	for(;;)
	{
		marker = fileContents.first[i];
		character = fileContents.first[i + 1];
		i += 2;

		if(marker == internalNodeMarker && character == nonNullCharacter)
		{
			break;
		}
		else if(marker == leafNodeMarker)
		{
			Node* leaf = new Node(character, 0);
			nodes.push(leaf);
		}
		else if(marker == internalNodeMarker)
		{
			Node* parent = new Node(character, 0); //Character will be null and unused.
			parent->right = remove(&nodes);
			parent->left = remove(&nodes);
			nodes.push(parent);
			Node p = *parent;
		}
	}

	return std::make_pair(remove(&nodes), i);
}

/**
 * Removes and returns the top node in the stack.
 * 
 * @param The stack to remove from.
 * @return A reference to the top node.
 */
Node* remove(std::stack<Node*>* nodes)
{
	Node* node = nodes->top();
	nodes->pop();

	return node;
}

/**
 * Decodes the rest of the file using the Huffman tree.
 *
 * @param root The root of the Huffman tree.
 * @param index The index of where the compressed representation begins in fileContents.
 * @param fileContents The contents of the compressed file.
 * @param file The file to write the decompressed representation to.
 */
void decompress(Node* root, size_t index, std::pair<char*, std::size_t> fileContents, std::ofstream& file)
{
	const unsigned int BYTE_LENGTH = 8;
	const char LEFT_BRANCH = 0;
	char character = ' ';
	bool secondToLastIteration = false;
	unsigned int numberOfBits = 0;
	Node* node = root;

	for(unsigned int i = index; i < fileContents.second; i++)
	{
		character = fileContents.first[i];

		//The second to last byte will hold the number of valid (non-padding) bits in the last byte.
		secondToLastIteration = ( i == (fileContents.second - 2) );

		//On the last byte, we want to use the old value of numberOfBits.
		if(i != fileContents.second - 1)
		{
			numberOfBits = (secondToLastIteration)? (unsigned int) character : BYTE_LENGTH;
		}

		if(!secondToLastIteration)
		{
			for(unsigned int n = 0; n < numberOfBits; n++)
			{
				char currentBit = character & (0x80 >> n);
			
				if(currentBit == LEFT_BRANCH)
				{
					node = node->left;
				}
				else
				{
					node = node->right;
				}

				if(node->isLeaf())
				{
					file << node->character;
					node = root;
				}
			}
		}
	}
}

/**
 * Decodes a file that has been huffman encoded.
 *
 *@param filename The name of the Huffman encoded file.
 */
void huffmanDecode(char* filename)
{
	std::string outputFilename = filename;
	outputFilename = outputFilename.substr(0, strlen(filename) - 4); //Remove the .enc at the end.
	std::ofstream file(outputFilename, std::ios::binary);
	
	if(!file.is_open())
	{
		std::cerr << "Could not open: " << outputFilename << " for writing" << std::endl;
		exit(1);
	}

	std::pair<char*, size_t> fileContents = getCharactersFromFile(filename);
	std::pair<Node*, size_t> nodeAndIndex = getTree(fileContents);
	decompress(nodeAndIndex.first, nodeAndIndex.second, fileContents, file);
	
	delete[] fileContents.first;
	deleteNodes(nodeAndIndex.first);
}
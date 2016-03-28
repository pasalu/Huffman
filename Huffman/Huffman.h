#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <utility>
#include <map>
#include <set>
#include <string>
#include <bitset>
#include <stack>
#include "Node.h"

std::pair<char*, std::size_t> getCharactersFromFile(char* filename);
std::map<char, unsigned int> getCharactersAndFrequencies(const std::pair<char*, std::size_t>& characters);
Node* removeLowestFrequency(std::multiset<Node, NodeCompare>* nodes);
Node* constructHuffmanTree(const std::map<char, unsigned int>& charactersAndFreuencies);
void getCodes(Node* root, std::string s, std::map<char, std::string>* codes);
std::string padWithZeros(std::string s);
void writeCodesToFile(std::ofstream& file, const std::map<char, std::string>& codes, const std::pair<char*, std::size_t>& characters);
void writeTreeToFile(std::ofstream& file, Node* root);
void writeTreeRecursively(std::ofstream& file, Node* root);
void deleteNodes(Node* root);
void huffmanEncode(char* filename);

Node* remove(std::stack<Node*>* nodes);
std::pair<Node*, size_t> getTree(std::pair<char*, std::size_t> fileContents);
void decompress(Node* root, size_t index, std::pair<char*, std::size_t> fileContents, std::ofstream& file);
void huffmanDecode(char* filename);

#endif HUFFMAN_H
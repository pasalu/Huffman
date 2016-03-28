/**
 * @author Peter Salu
 * 
 * @Description A program to Huffman encode and decode files.
 * System ID Number: 15803
 */

#include <iostream>
#include "Huffman.h"

/**
 * Prints instructions on how to run the program.
 */
void printUsage()
{
	std::cout << "Usage: A program to Huffman encode or decode files." << std::endl << std::endl
				<< "To encode: Huffman.exe -e filename" << std::endl
				<< "Output will be in filename.enc" << std::endl << std::endl
				<< "To decode: Huffman.exe -d filename" << std::endl
				<< "Output will be filename without the \".enc\" at the end" << std::endl;
}

/**
 * Checks to see if a string ends in ".enc"
 *
 * @param filename the string to check.
 * @return true if it ends in .enc, false otherwise.
 */
bool endsInEnc(char* filename)
{
	std::string s = filename;
	const unsigned int charactersInEnc = 4;

	if(s.length() < charactersInEnc)
	{
		return false;
	}

	return s.substr(s.length() - charactersInEnc) == ".enc";
}

/**
 * Parses the flags and performs either Huffman encoding or decoding.
 *
 * @param argv The command line arguments.
 */
void parseFlagsAndRun(char** argv)
{
	const unsigned int indexOfFlag = 1;
	const unsigned int indexOfFilename = 2;

	char* filename = argv[indexOfFilename];

	if(strcmp(argv[indexOfFlag], "-e") == 0)
	{
		huffmanEncode(filename);
	}
	else if(strcmp(argv[indexOfFlag], "-d") == 0)
	{
		if(endsInEnc(filename))
		{
			huffmanDecode(filename);
		}
		else
		{
			std::cout << "Input file was not encoded by this program." << std::endl << std::endl;
			printUsage();
		}
	}
	else
	{
		std::cout << "Unknown flag \"" << argv[indexOfFlag] << "\"" << std::endl << std::endl;
		printUsage();
	}
}

int main(int argc, char** argv)
{
	const unsigned int properNumberOfArguments = 3;

	if(argc != properNumberOfArguments)
	{
		printUsage();
		exit(1);
	}
	else
	{
		parseFlagsAndRun(argv);
	}

	return 0;
}
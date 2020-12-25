#ifndef _HUFF_H_
#define _HUFF_H_

#include<fstream>
#include<string>
#include<array>
#include<iostream>
#include<vector>
#include<cmath>

#define MAX_CODE 44
#define ALPHABET_SIZE 8

class TREE   
{
public:
	int freq;
	unsigned char let;
	TREE *left;
	TREE *right;
   
};

bool ArgsComand(char* argv[], int argc);
void EncodeFile(std::string fromwher, std::string towher);
void DecodeFile(std::string fromwher, std::string towher);

#endif 

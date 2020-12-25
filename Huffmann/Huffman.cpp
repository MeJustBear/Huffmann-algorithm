#include"Huffman.h"

#include<climits>
#include<stdexcept>

static std::vector<char *> codes;
static char code[MAX_CODE];

/**
@brief ArgsComand analysis 
@param argv (array if chars*)
@param argc (numbers of chars*)
*/
bool ArgsComand(char* argv[], int argc) {
	if ( argv[1][1]=='h'){
		throw std::runtime_error("\ntype -e/-d [input_filename/coded_filename] [encoded_filename/output_filename]\n");
	}
	return argv[1][1] == 'e';
}

/**
@brief Countfreak Simple count freak for symbols in file
@param freak (array of freaks)
@param filename (name of file)
*/
void Countfreak(std::vector<int> &freak, std::string filename) {
	int p = pow(2, ALPHABET_SIZE);
	for (int i = 0; i < p; i++) {
		freak.push_back(0);
	}
	std::ifstream fin(filename, std::ios::binary);
	for (unsigned char i = 0; !fin.eof(); ) {
		fin.read((char *)&i, sizeof(i));
		if (!fin.eof() )
			freak[i] ++;
	}
}


/**
@brief InsertToForest  function of inserting in forest
@param forest (array of TREE)
@param forestsize (size of forest)
@param newEL  (element that we need to insert)
*/
void InsertToForest(std::vector <TREE *> &forest, int forestsize, TREE *newEL) {
	int pos;
	for (pos = forestsize; pos > 0 && newEL->freq > forest[pos-1]->freq; pos--)
		forest[pos] = forest[pos - 1];
	forest[pos] = newEL;
}


/**
@brief Mergeforest function of merge forest
@param forest (array of TREE)
@param size (size of forest)
*/
void Mergeforest(std::vector <TREE *> &forest, int size) {
	int pos;
	for (pos = size - 1; pos > 0; pos--) {
		TREE *newEL = new TREE;
		if (newEL == NULL) {
			fprintf(stderr, "Could not allocate %u bytes\n", sizeof(TREE));
			return;
		}
		newEL->let = 2;
		newEL->freq = forest[pos - 1]->freq + forest[pos]->freq;
		newEL->left = forest[pos - 1];
		newEL->right = forest[pos];
		InsertToForest(forest, pos - 1, newEL);
	}
}

/** 
@brief BuildTREE
@param freq (array of freak simbols)
@return *TREE (pointer on TREE with all symbols)
*/
TREE *BuildTREE(std::vector<int> &freq) {
	std::vector <TREE *> forest;
	int p = pow(2, ALPHABET_SIZE);
	forest.reserve(p/*(int)pow(2, ALPHABET_SIZE)*/);
	for (int i = 0; i < p/*(int)pow(2, ALPHABET_SIZE)*/; i++)
		forest.push_back(NULL);
	int c, size = 0;
	for (c = 0; c < p /*(int)pow(2, ALPHABET_SIZE)*/; c++) {

		TREE *newEL = new TREE;
		if (newEL == NULL) {
			fprintf(stderr, "Could not allocate %u bytes\n", sizeof(TREE));
			return NULL;
		}
		newEL->let = c;
		newEL->freq = freq[(int)c];
		newEL->left = newEL->right = NULL;
		if (newEL->freq > 0) {
			InsertToForest(forest, size, newEL);
			size++;
		}
	}

	Mergeforest(forest, size);


	return forest[0];
}


/**
@brief TraverseTREE function of building codes for symbols
@param T (element of forest)
@param deph (deph of element)
*/
int TraverseTREE(TREE *T, int deph) {
	if (T->left == NULL || T->right == NULL) {
		code[deph] = 0;
		codes[T->let] = _strdup(code);
	}
	else {
		code[deph] = '0';
		TraverseTREE(T->right, deph + 1);
		code[deph] = '1';
		TraverseTREE(T->left, deph + 1);
	}
	return 1;
} 

/**
@brief FillBuff helping function for writig by bytes
*/
char FillBuff(int &w, int &cod, unsigned char &buff,unsigned char c) {
	for (; codes[c][cod] != NULL && w < ALPHABET_SIZE; cod++){
		if (codes[c][cod] == '1')
			buff = buff | (1 << ALPHABET_SIZE - w-1);
		w++;
	}
		return buff;
}

/**
@brief Load function of saving huffmans file 
*/
void Load(std::vector<int> &freq, std::string fromwher, std::string towher) {
	std::ofstream of(towher, std::ios::binary);
	unsigned char c;
	std::ifstream fin(fromwher, std::ios::binary);

	fin.seekg(0, std::ios::end);
	unsigned long size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	of.write((char*)&size, sizeof(size));
	c = ALPHABET_SIZE;
	of.write((char*)&c, sizeof(unsigned char));
	int p = pow(2, ALPHABET_SIZE);
	for (int i = 0; i < p; i++)
		of.write((char*)&freq[i],sizeof(freq[i]));
	
	fin.read((char *)&c, sizeof(c));
	unsigned char buff = 0;
	int cod = 0;
	int word_moves = 0;
	while (!fin.eof()) {
		if (c == -1)
			break;

		while (word_moves <= ALPHABET_SIZE-1 && !fin.eof()) {
			FillBuff(word_moves, cod, buff, c);
			if (word_moves <= ALPHABET_SIZE-1) {
				fin.read((char *)&c, sizeof(c));
				cod = 0;
			}
		}
		if (codes[c][cod] == NULL) {
			fin.read((char *)&c, sizeof(c));
			cod = 0;
		}		
		word_moves = 0;
		of.write((char*)&buff,sizeof(buff));
		buff = 0;
	}
	fin.close();
	of.close();

}

/**
@brief EncodeFile function for start encoding
@param fromwher (file what we will use)
@param towher (file where we will write)
*/
void EncodeFile(std::string fromwher, std::string towher) {
	int p = pow(2, ALPHABET_SIZE);
	codes.reserve( p);
	for (int i = 0; i <  p; i++)
		codes.push_back(NULL);
	std::vector<int> freak;
	freak.reserve( p);
	TREE *root;
	Countfreak(freak, fromwher);
	root = BuildTREE(freak);
	TraverseTREE(root, 0);
	Load(freak, fromwher, towher);
	delete[]root;
}

/**
@brief Countfreq2 function for count freak from our huffman file
@param freq (array of freq)
@param fin (input file)
@param size (size of freq)
*/
void Countfreq2(std::vector<int> &freq, std::ifstream &fin,unsigned long &size){
	unsigned char o;
	fin.read((char*)&size,sizeof(size));
	fin.read((char*)&o, sizeof(o));
	int p = pow(2, ALPHABET_SIZE);
	for (int i = 0; i < p; i++) {
		freq.push_back(0);
	}

	for (int i = 0; i < (int)pow(2, ALPHABET_SIZE); i++) {
		fin.read((char*)&freq[i], sizeof(freq[i]));
	}
}

/**
@brief ReadBuff helping function for reading from huffman file
*/
void ReadBuff(int &w, unsigned char &buff, TREE *(&T)) {
	while (w<ALPHABET_SIZE) {
		bool cur_bit = buff & (1 << ALPHABET_SIZE - 1 - w);
		if ( cur_bit == false && T->right != NULL)
			T = T->right;
		else if ( cur_bit && T->left != NULL)
			T = T->left;
		w++;
		if (T->left == NULL && T->right == NULL) {
			break;
		}

	}
}

/**
@brief Decode function of decoding huffman file
@param fin (huffman file)
@param towher (where decode)
@param root (tree of elements)
*/

void Decode(TREE * root, std::ifstream &fin, std::string towher, unsigned long Size){
	unsigned char c;
	unsigned long size = 0;
	TREE *T = root;
	std::ofstream of(towher, std::ios::out | std::ios::binary );
	int w;
	w = 0;
	while (!fin.eof() && size<Size) {

		if (w == 0) {
			c = 0;
			fin.read((char*)&c, 1);
		}
		while (!(T->left == NULL && T->right == NULL)) {
			ReadBuff(w, c, T);
			if ( !(T->left == NULL && T->right == NULL)) {
				c = 0;
				fin.read((char*)&c, 1);
			}
			if(w == ALPHABET_SIZE)
				w = 0;
		}
		of << T->let;
		size++;
		T = root;
	}
}

/**
@brief DecodeFile function of preparate for decode
@param fromwher (huffman file)
@param towher (where decode)
*/
void DecodeFile(std::string fromwher, std::string towher){
	TREE *root;
	std::vector<int> freq;
	int p = pow(2, ALPHABET_SIZE);
	freq.reserve(p);
	std::ifstream fin(fromwher, std::ios::binary);
	unsigned long size = 0;
	Countfreq2(freq, fin, size);
	root = BuildTREE(freq);
	Decode(root, fin, towher, size);
}
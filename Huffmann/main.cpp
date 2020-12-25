#include"Huffman.h"



int main(int argc, char* argv[]){
	setlocale(1,"ru");
	bool argcheck;
	try {
		argcheck=ArgsComand(argv, argc);
	}
	catch (std::runtime_error& ex) {
		std::cerr << ex.what();
		system("pause");
		return 0;
	}
	if (argcheck)
		EncodeFile(argv[2], argv[3]);
	else DecodeFile(argv[2], argv[3]);
	system("pause");
	return 0;
}
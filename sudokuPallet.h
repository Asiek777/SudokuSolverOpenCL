#pragma once
#include <fstream>
#include <iostream>
#include <CL/cl.hpp>
#ifdef __GNUC__
#define PACKED( class_to_pack ) class_to_pack __attribute__((__packed__))
#else
#define PACKED( class_to_pack ) __pragma( pack(push, 1) ) class_to_pack __pragma( pack(pop) )
#endif
#define SUCCESS 0
#define FAILURE 1
#define EXPECTED_FAILURE 2

int fileToString(const char *filename, std::string& s) {
	size_t size;
	char*  str;

	// create a file stream object by filename
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));


	if (!f.is_open()) {
		return FAILURE;
	}
	else {
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);

		str = new char[size + 1];
		if (!str) {
			f.close();
			return FAILURE;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';

		s = str;
		delete[] str;
		return SUCCESS;
	}
}

//PACKED(
typedef struct sudokuPallet {
	cl_char numbers[9][9];
} sudokuPallet;
//)


sudokuPallet fileToSudoku(char * filename){
	std::string s;
	fileToString(filename, s);
	int len = s.size();
	sudokuPallet pallet;
	for (int i = 0; i < 81; i++) {
		pallet.numbers[i / 9][i % 9] = s[i] == 'x' ? '0' : s[i];
	}
	for (int i = 0; i < 81; i++) {
		pallet.numbers[i / 9][i % 9] -= 48;
	}
	return pallet;
}

void printPallet(std::string header, sudokuPallet pallet) {
	std::cout << header << std::endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++)
			std::cout << (char)(pallet.numbers[i][j] + 48);
		std::cout << std::endl;
	}
}

bool next0inPallet(sudokuPallet pallet, int &y, int &x) {
	x++;
	for (; y < 9; y++) {
		for (; x < 9; x++)
			if (pallet.numbers[y][x] == 0)
				return false;
		x = 0;
	}
	return true;
}

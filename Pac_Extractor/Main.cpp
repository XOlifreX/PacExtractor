#include <iostream>
#include <fstream>
#include <vector>


struct PACFileStructure {
	std::string name;
	int startAddress;
	int size;
	char* content;
};

// is the first byte of this int variable equal to one, then it is little Endian
bool isBigEndian() {
	int temp = 1;
	return !((char*)&temp)[0];
}

// char buffer to int conversion
int convertToInt(const char* bugger, size_t len) {

	int temp = 0;

	if (isBigEndian()) {
		for (int i = 0; i < len; i++)
			((char*)&temp)[3 - i] = bugger[i];
	}
	else
	{
		for (int i = 0; i < len; i++)
			((char*)&temp)[i] = bugger[i];
	}

	return temp;

}


int main() {

	char fourByteBuffer[4];
	std::ifstream ifs("../Pac_Extractor/bgm.pac", std::ios::binary | std::ios::in);

	// File type check
	ifs.read(fourByteBuffer, 4);

	if (strncmp(fourByteBuffer, "MGPK", 4) != 0) {
		std::cout << "Wrong file type." << std::endl;
		std::cin.get();

		ifs.close();

		return 1;
	}

	// empy
	ifs.read(fourByteBuffer, 4);

	// Amount of files
	ifs.read(fourByteBuffer, 4);
	int numOfFiles = convertToInt(fourByteBuffer, 4);
	
	PACFileStructure* files = new PACFileStructure[numOfFiles];

	std::cout << "Analysing contents." << std::endl;

	for (int i = 0; i < numOfFiles; i++) {

		// garbage?
		ifs.read(fourByteBuffer, 1);

		// file name 1st item
		char filename[32 + 1];
		ifs.read(filename, 31);
		filename[31] = '\0';
		files[i].name = filename;

		// start address 1st item
		ifs.read(fourByteBuffer, 4);
		files[i].startAddress = convertToInt(fourByteBuffer, 4);

		// size 1st element
		ifs.read(fourByteBuffer, 4);
		files[i].size = convertToInt(fourByteBuffer, 4);

		// offset to next file definition
		ifs.read(fourByteBuffer, 4);
		ifs.read(fourByteBuffer, 4);

	}
	
	std::cout << "Extracting files." << std::endl;

	for (int i = 0; i < numOfFiles; i++) {

		char* content = new char[files[i].size];
		ifs.read(content, files[i].size);

		// Eddit to change destination folder.
		auto output = std::fstream("../Pac_Extractor/files/bgm/" + files[i].name, std::ios::out | std::ios::binary);

		bool isEncrypted = (files[i].name.find(".png") != std::string::npos | files[i].name.find(".txt") != std::string::npos);
		std::string fileNameNoExtension = files[i].name.substr(0, files[i].name.find("."));

		for (int j = 0; j < files[i].size; j++) {
			//if (isEncrypted)
				//*(content + j) = *(content + j) ^ (fileNameNoExtension[j % fileNameNoExtension.length()]);
			output.write((char*)(content + j), sizeof(char));
		}

		output.close();

		std::cout << "File " << (i+1) << " created." << std::endl;

		files[i].content = content;

	}

	ifs.close();


	for (int i = 0; i < numOfFiles; i++)
		free(files[i].content);

	std::cout << "File(s) created successfully." << std::endl;

	std::cin.get();

	return 0;

}
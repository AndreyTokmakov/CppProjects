//============================================================================
// Name        : Streams.h
// Created on  : 12.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Streams src class
//============================================================================

#include "Streams.h"

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include <vector>
#include <string>
#include <algorithm>

namespace {
	const static std::string DATA_FILE = R"(C:\Temp\cities.txt)";
	const static std::string File_Tp_Write = "S:\\Temp\\Folder_For_Testing\\data\\File_Tp_Write.txt";
}

namespace Streams::Ifstream {

	void ReadFile() {
		std::ifstream ifs;
		ifs.open(DATA_FILE, std::ifstream::in);

		char c = ifs.get();

		while (ifs.good()) {
			std::cout << c;
			c = ifs.get();
		}
		ifs.close();
	}

	void Read_File_GOOD() {
		std::ifstream file{ DATA_FILE};

		std::vector<std::string> lines;
		while (std::getline(file, lines.emplace_back())) {
		};

		std::for_each(lines.cbegin(), lines.cend(), [](const auto& s) {
			std::cout << "\"" << s << "\",";
		});
	}

	void ReadFile_Rdbuf() {
		std::ifstream ifs(DATA_FILE);
		if (false == ifs.bad()) {
			// Dump the contents of the file to cout.
			std::cout << ifs.rdbuf();
			ifs.close();
		}
	}
}


namespace Streams::Fstream {

	using ios = std::ios;

	void Write_and_Read()	{
		std::fstream fs(File_Tp_Write, ios::in | ios::out | ios::trunc);
		if (false == fs.bad()) {
			// Write to the file.
			fs << "Writing to a basic_fstream object..." << std::endl;
			fs.close();

			// Dump the contents of the file to cout.
			fs.open(File_Tp_Write, ios::in);
			std::cout << fs.rdbuf();
			fs.close();
		}
	}
}


namespace Streams::Streambuf {

	void ReadFile_Sgetc() {
		std::ifstream istr(DATA_FILE);
		if (istr) {
			std::streambuf * pbuf = istr.rdbuf();
			while (pbuf->sgetc() != EOF) {
				char ch = pbuf->sbumpc();
				std::cout << ch;
			}
			istr.close();
		}
	}
}


void Streams::TestAll() {
	// Ifstream::ReadFile();
	// Ifstream::ReadFile_Rdbuf();

	Ifstream::Read_File_GOOD();

	// Streambuf::ReadFile_Sgetc();

	// Fstream::Write_and_Read();
};

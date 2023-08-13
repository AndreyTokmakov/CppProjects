//============================================================================
// Name        : FilesStreams.cpp
// Created on  : 09.10.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Files and streams src
//============================================================================

#include "FilesStreams.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>	
#include <functional>
#include <filesystem>
#include <cassert>

namespace FilesStreams {

	namespace {
		inline static std::string file_path = R"(S:\Temp\Folder_For_Testing\dump_file)";
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& ostr, const std::vector<T>& list) {
		for (const auto &i : list)
			ostr << " " << i;
		return ostr;
	}
}

namespace FilesStreams::Serialize_Vector {

	void Serialize_Vector() {
		std::vector<int> output{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		// Write vector to file:
		std::ofstream ofile(file_path.c_str(), std::ios::binary);
		if (ofile.is_open()) {
			ofile.write(reinterpret_cast<char*>(output.data()), output.size() * sizeof(int));
		} else {
			return;
		}
		ofile.close();

		std::vector<int> input;
		std::cout << "Before: " << input << std::endl;
		
		// Read vector from file:
		std::ifstream ifile(file_path.c_str(), std::ios::binary);
		if (ifile.is_open()) {
			ifile.seekg(0, std::ios_base::end);
			auto bytes_length = ifile.tellg();
			ifile.seekg(0, std::ios_base::beg);
			input.resize(static_cast<size_t>(bytes_length / sizeof(int)));

			// Read the content of the file to the allocated buffer by providing a pointer to the array of characters
			ifile.read(reinterpret_cast<char*>(input.data()), bytes_length);

			// Check that the read operation is completed successfully:
			assert(false == ifile.fail());
			assert(bytes_length == ifile.gcount());
			//auto success = !ifile.fail() && bytes_length == ifile.gcount();
		}
		ifile.close();
		std::cout << "After: " << input << std::endl;
	}

	//--------------------------------------------------------------------------------------------//

	bool write_data(const char* const filename,
					const char* const data,
		            const size_t size) {
		auto success = false;
		std::ofstream ofile(filename, std::ios::binary);
		if (ofile.is_open()) {
			try {
				ofile.write(data, size);
				success = true;
			} catch (std::ios_base::failure &) {
				// handle the error
			}
			ofile.close();
		}
		return success;
	}

	size_t read_data(const char* const filename,
		             std::function<char*(size_t const)> allocator)
	{
		size_t readbytes = 0;
		std::ifstream ifile(filename, std::ios::ate | std::ios::binary);
		if (ifile.is_open()) {
			auto length = static_cast<size_t>(ifile.tellg());
			ifile.seekg(0, std::ios_base::beg);
			auto buffer = allocator(length);
			try {
				ifile.read(buffer, length);
				readbytes = static_cast<size_t>(ifile.gcount());
			} catch (std::ios_base::failure &) {
				// handle the error
			}
			ifile.close();
		}
		return readbytes;
	}

	//-------------------------------------------------------------------------------------------------------//

	template<typename T>
	bool serialize(std::vector<T>& data,
				   const std::string& filename = std::string{ R"(S:\Temp\Folder_For_Testing\dump_file)" }) {
		auto result{ false };
		std::ofstream file(filename.c_str(), std::ios::binary);
		if (true == file.is_open()) {
			try {
				file.write(reinterpret_cast<char*>(data.data()), 
					       data.size() * sizeof(T));
				result = true;
			} catch (const std::ios_base::failure&) {
				// handle the error
			}
		}
		file.close();
		return result;
	}

	template<typename T>
	bool deserialize(std::vector<T>& data,
		             const std::string& filename = std::string{ R"(S:\Temp\Folder_For_Testing\dump_file)" }) {
		auto result{ false };

		// Read vector from file:
		std::ifstream file(filename.c_str(), std::ios::binary);
		if (file.is_open()) {
			file.seekg(0, std::ios_base::end);
			auto bytes_length = file.tellg();
			file.seekg(0, std::ios_base::beg);
			data.resize(static_cast<size_t>(bytes_length / sizeof(T)));

			// Read the content of the file to the allocated buffer by providing a pointer to the array of characters
			file.read(reinterpret_cast<char*>(data.data()), bytes_length);

			// Check that the read operation is completed successfully:
			assert(false == file.fail());
			assert(bytes_length == file.gcount());
			//auto success = !ifile.fail() && bytes_length == ifile.gcount();
		}
		file.close();

		return result;
	}

	void Serialize_Vector_2() {
		std::vector<int> src{ 0, 4, 2, 3, 4, 5, 6, 7, 8, 9 }, dst;

		serialize(src);
		std::cout << "Before: " << dst << std::endl;

		deserialize(dst);
		std::cout << "After: " << dst << std::endl;
	}
};


namespace FilesStreams::SerializeComplexClass {

	class Foo {
	public:
		int i;
		char c;
		std::string s;

	public:
		Foo(int i = 0, char c = 0, const std::string& s = {}): i(i), c(c), s(s) {
		}
		Foo(const Foo&) = default;
		Foo& operator=(const Foo& obj) = default;

		bool operator==(const Foo& obj) const {
			return i == obj.i && c == obj.c && s == obj.s;
		}

		bool operator!=(const Foo& obj) const {
			return !(*this == obj);
		}

		friend std::ofstream& operator<<(std::ofstream& ofile, const Foo& foo);
		friend std::ifstream& operator>>(std::ifstream& ifile, Foo& foo);

	protected:
		bool __serialize(std::ofstream& ofile) const {
			ofile.write(reinterpret_cast<const char*>(&this->i), sizeof(this->i));
			ofile.write(&this->c, sizeof(this->c));
			auto size = static_cast<int>(this->s.size());
			ofile.write(reinterpret_cast<char*>(&size), sizeof(size));
			ofile.write(this->s.data(), this->s.size());
			return !ofile.fail();
		}

		bool __deserialize(std::ifstream& ifile)
		{
			ifile.read(reinterpret_cast<char*>(&i), sizeof(i));
			ifile.read(&c, sizeof(c));
			auto size{ 0 };
			ifile.read(reinterpret_cast<char*>(&size), sizeof(size));
			s.resize(size);
			ifile.read(reinterpret_cast<char*>(&s.front()), size);
			return !ifile.fail();
		}

	public:
		bool serialize(const std::string& filename = std::string{ R"(S:\Temp\Folder_For_Testing\dump_file)" }) {
			auto result{ false };
			std::ofstream file(filename.c_str(), std::ios::binary);
			if (true == file.is_open()) {
				try {
					result = this->__serialize(file);
				}
				catch (const std::ios_base::failure&) {
					// handle the error
				}
			}
			file.close();
			return result;
		}

		bool deserialize(const std::string& filename = std::string{ R"(S:\Temp\Folder_For_Testing\dump_file)" }) {
			auto result{ false };
			std::ifstream file(filename.c_str(), std::ios::binary);
			if (true == file.is_open()) {
				try {
					result = this->__deserialize(file);
				}
				catch (const std::ios_base::failure&) {
					// handle the error
				}
			}
			file.close();
			return result;
		}
	};

	std::ofstream& operator<<(std::ofstream& ofile, const Foo& foo) {
		ofile.write(reinterpret_cast<const char*>(&foo.i), sizeof(foo.i));
		ofile.write(&foo.c, sizeof(foo.c));
		auto size = static_cast<int>(foo.s.size());
		ofile.write(reinterpret_cast<char*>(&size), sizeof(size));
		ofile.write(foo.s.data(), foo.s.size());
		return ofile;
	}

	std::ifstream& operator>>(std::ifstream& ifile, Foo& foo) {
		ifile.read(reinterpret_cast<char*>(&foo.i), sizeof(foo.i));
		ifile.read(&foo.c, sizeof(foo.c));
		auto size{ 0 };
		ifile.read(reinterpret_cast<char*>(&size), sizeof(size));
		foo.s.resize(size);
		ifile.read(reinterpret_cast<char*>(&foo.s.front()), size);
		return ifile;
	}

	//--------------------------------------------------------------------------//


	void Test_1() {
		Foo foo(1, 'a', "One"), dest;
		std::cout << "foo  :    " << foo.s << " : [" << foo.i << ", " << foo.c << "]" << std::endl;
		std::cout << "dest :    " << dest.s << " : [" << dest.i << ", " << dest.c << "]" << std::endl;

		std::cout << "serialize_foo = " << std::boolalpha << foo.serialize() << std::endl;
		std::cout << "deserialize_foo = " << std::boolalpha << dest.deserialize()<< std::endl;

		std::cout << "foo  :    " << foo.s << " : [" << foo.i << ", " << foo.c << "]" << std::endl;
		std::cout << "dest :    " << dest.s << " : [" << dest.i << ", " << dest.c << "]" << std::endl;
	}


	bool serialize_foo(const Foo& foo, 
		               const std::string& filename = std::string{ R"(S:\Temp\Folder_For_Testing\dump_file)" }) {
		auto result{ false };
		std::ofstream file(filename.c_str(), std::ios::binary);
		if (true == file.is_open()) {
			try {
				file << foo;
				result = true;
			}
			catch (const std::ios_base::failure&) {
				// handle the error
			}
		}
		file.close();
		return result;
	}

	bool deserialize_foo(Foo& foo,
					     const std::string& filename = std::string{ R"(S:\Temp\Folder_For_Testing\dump_file)" }) {
		auto result{ false };
		std::ifstream file(filename.c_str(), std::ios::binary);
		if (true == file.is_open()) {
			try {
				file >> foo;
				result = true;
			}
			catch (const std::ios_base::failure&) {
				// handle the error
			}
		}
		file.close();
		return result;
	}

	void Test_2() {
		Foo foo(2, 'b', "Two"), dest;
		std::cout << "foo  :    " << foo.s << " : [" << foo.i << ", " << foo.c << "]" << std::endl;
		std::cout << "dest :    " << dest.s << " : [" << dest.i << ", " << dest.c << "]" << std::endl;

		auto result = serialize_foo(foo);
		std::cout << "serialize_foo = " << std::boolalpha << result << std::endl;

		result = deserialize_foo(dest);
		std::cout << "deserialize_foo = " << std::boolalpha << result << std::endl;

		std::cout << "foo  :    " << foo.s << " : [" << foo.i << ", " << foo.c << "]" << std::endl;
		std::cout << "dest :    " << dest.s << " : [" << dest.i << ", " << dest.c << "]" << std::endl;
	}
}

namespace FilesStreams::Readers {

	void _read_file(const std::string& file) {
		std::ifstream hFile(file, std::ios::in);
		if (false == hFile.is_open()) {
			std::cout << "Can't open file " << file << "\n";
			return;
		}

		std::stringstream buffer;
		buffer << hFile.rdbuf();
		std::cout << buffer.str() << std::endl;
	}

	void SimpleReadFile() {
		_read_file(R"(S:\Temp\Folder_For_Testing\File_11.txt)");
	}

	//-----------------------------------------------------------

	void SimpleReadFile2() {
		std::ifstream file{ R"(D:\Temp\Folder_For_Testing\data\Some_File_Widh_Data.txt)" };

		std::string line;
		while (std::getline(file, line)) {
			std::cout << line << std::endl;
		}
	}

	//-----------------------------------------------------------

	void SimpleReadFile_BAD() {
		std::ifstream file{ R"(D:\Temp\Folder_For_Testing\data\Some_File_Widh_Data.txt)" };
		std::vector<std::string> lines;

		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(std::move(line));
		}

		std::for_each(lines.cbegin(), lines.cend(), [](const auto& s) {std::cout << s << std::endl; });
	}

    void ReadFileLinesToVector()
    {
        size_t blockSize = 5;
        std::vector<std::string> lines;
        constexpr std::string_view path { R"(/home/andtokm/tmp/TEST_FILES/TestFile.txt)"};

        if (std::ifstream file(path.data(), std::ios::binary); file.is_open() && file.good()) {
            int bytes { 0 };
            std::string& buffer = lines.emplace_back(blockSize, '0');
            while (0 < (bytes = file.read(buffer.data(), blockSize).gcount())) {
                if (blockSize > bytes)
                    buffer.resize(bytes);
                std::cout << bytes << std::endl;
                std::cout << buffer << std::endl;
            }
        }
    }

    void ReadFileBlocks()
    {
        constexpr std::string_view file_path
                { R"(/home/andtokm/DiskS/Temp/TESTING_ROOT_DIR/test_run.log)"};

        size_t bytesReadTotal { 0 };
        char buffer[1024 * 1024] {};
        if (std::ifstream file(file_path.data(), std::ios::binary); file.is_open() && file.good()) {
            std::streamsize bytesRead {0};
            while (0 < (bytesRead = file.readsome(buffer, std::size(buffer)))) {
                // std::cout << bytesRead << std::endl;
                bytesReadTotal += bytesRead;
            }
        }

        std::cout << "Bytes read: " << bytesReadTotal << std::endl;
        std::cout << "File size : " << std::filesystem::file_size(file_path) << std::endl;
    }

}

void FilesStreams::TestAll() {
	// Serialize_Vector::Serialize_Vector();
	// Serialize_Vector::Serialize_Vector___Error();
	// Serialize_Vector::Serialize_Vector_2();

	// SerializeComplexClass::Test_1();
	// SerializeComplexClass::Test_2();

	// Readers::SimpleReadFile();
	// Readers::SimpleReadFile2();
	// Readers::SimpleReadFile_BAD();
	Readers::ReadFileLinesToVector();
	Readers::ReadFileBlocks();
};
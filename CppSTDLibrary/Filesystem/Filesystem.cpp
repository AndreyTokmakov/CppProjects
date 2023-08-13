//============================================================================
// Name        : Filesystem.cpp
// Created on  : 2021-11-06.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Filesystem
//============================================================================

#include "Filesystem.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <optional>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <set>
#include <iterator>


namespace Filesystem
{
    void DirectoryIteratorTest(int level = 0)
    {
        const std::string dirPath = R"(/home/andtokm/tmp/folder_for_testing)";
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            const auto filenameStr = entry.path().filename().string();
            if (entry.is_directory()) {
                std::cout << std::setw(level * 3) << "" << filenameStr << '\n';
                // DisplayDirectoryTree(entry, level + 1);
            }
            else if (entry.is_regular_file()) {
                std::cout << std::setw(level * 3) << ""<< filenameStr
                          << ", size " << std::filesystem::file_size(entry) << " bytes\n";
            }
            else
                std::cout << std::setw(level * 3) << "" << " [?]" << filenameStr << '\n';

        }
    }

    void DirectoryIteratorTest_Recursive() {
        const std::string dirPath = R"(/home/andtokm/tmp/folder_for_testing)";
        for (const auto& path : std::filesystem::recursive_directory_iterator(dirPath)) {
            std::cout << path.path() << std::endl;
        }
    }

    void DirectoryIteratorTest_Recursive_WithExtension()
    {
        constexpr std::string_view dir { R"(S:\Projects\cpp\third_party\VTK)" },  ext { ".h"};

        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
            if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ext
                    ) {
                std::cout << entry.path() << std::endl;
            }
        }
    }

    void Enumerate_Directory() {
        const std::filesystem::path dir = R"(S:\Temp\Folder_For_Testing)";
        if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
            for (auto const & entry : std::filesystem::recursive_directory_iterator(dir)) {
                auto filename = entry.path().filename();
                if (std::filesystem::is_directory(entry.status()))
                    std::cout << "[+]" << filename << std::endl;
                else if (std::filesystem::is_symlink(entry.status()))
                    std::cout << "[>]" << filename << std::endl;
                else if (std::filesystem::is_regular_file(entry.status()))
                    std::cout << " " << filename << std::endl;
                else
                    std::cout << "[?]" << filename << std::endl;
            }
        }
    }


    void CreateDirectoryTest() {
        namespace fs = std::filesystem;

        constexpr std::string_view dir1 = R"(C:\Temp\TEST_DIRS\DirToCreate1)";
        bool result = fs::create_directory(dir1);
        std::cout << "Create directory result : " << result << std::endl;

        // Check is dir exists:
        if (fs::exists(dir1)) {
            std::cout << "Directory '" << dir1 << "' exists" << std::endl;
        }

        constexpr std::string_view dirs = R"(C:\Temp\TEST_DIRS\DirToCreate1\DirToCreate2\DirToCreate3)";
        result = fs::create_directories(dirs);
        std::cout << "Create directories result : " << result << std::endl;

        // Check is dir exists:
        if (fs::exists(dir1)) {
            std::cout << "Directories '" << dirs << "' exists" << std::endl;
        }
    }

    void DeleteDirectoryTest() {
        namespace fs = std::filesystem;

        constexpr std::string_view dir1 = R"(C:\Temp\TEST_DIRS\DirToCreate1)";
        constexpr std::string_view  dirs = R"(C:\Temp\TEST_DIRS\DirToCreate1\DirToCreate2\DirToCreate3)";

        try {
            bool result = fs::remove(dir1);
            std::cout << "Remove directory '" << dir1 << "' result : " << result << std::endl;
        }
        catch (const std::exception& exc) {
            std::cout << "Failed to delete dir '" << dir1 << "'." << std::endl;
            std::cout << exc.what() << std::endl;
        }

        try {
            bool result = fs::remove_all(dir1);
            std::cout << "RemoveAll directories '" << dir1 << "' result : " << result << std::endl;
        }
        catch (const std::exception& exc) {
            std::cout << "Failed to delete dir '" << dir1 << "'." << std::endl;
            std::cout << exc.what() << std::endl;
        }
    }

    void TempDirectoryPath() {
        namespace fs = std::filesystem;
        fs::path tempDir = fs::temp_directory_path();
        std::cout << "Temporary directory path : " << tempDir << std::endl;
    }

    void CopyDirTest() {
        namespace fs = std::filesystem;

        constexpr std::string_view srcDir = R"(C:\Temp\TEST_DIRS\html_dir)";
        constexpr std::string_view dstDir = R"(C:\Temp\TEST_DIRS\html_dir_copy)";

        std::error_code code;

        fs::copy(srcDir, dstDir, code);
        if (0 == code.value())
            std::cout << "'" << srcDir << "' copied to '" << dstDir << "' OK." << std::endl;

        std::cout << "\nAttempt 2...\n" << std::endl;

        fs::copy(srcDir, dstDir, code);
        if (0 != code.value())
            std::cout << "Failed to copy dir'" << srcDir << "' to '" << dstDir << "'. Error code = " << code.value() << std::endl;

    }

    void StandartMethods2() {

        namespace fs = std::filesystem;
        using namespace std::chrono_literals;

        const std::string filePath = "C:\\Temp\\FILES\\TestFile.txt";
        fs::path file(filePath);

        std::cout << "Root path (file) : " << file.root_path() << std::endl;
        std::cout << "Root name (file) : " << file.root_name() << std::endl;
        std::cout << "Relative Path (file) : " << file.relative_path() << std::endl;
        std::cout << "File name (file) : " << file.filename() << std::endl;
        std::cout << "Full path (file) : " << file.root_path().string() + file.relative_path().string() << std::endl;

        std::cout << "--------------------------------------------------------------------" << std::endl;

        fs::path currentPath = fs::current_path();

        std::cout << "Root path (current path) : " << currentPath.root_path() << std::endl;
        std::cout << "Root name (current path) : " << currentPath.root_name() << std::endl;
        std::cout << "Relative Path (current path) : " << currentPath.relative_path() << std::endl;
        std::cout << "File name (current path) : " << currentPath.filename() << std::endl;

        /*
        std::ofstream(p).put('a'); // create file of size 1
        std::cout << "File size = " << fs::file_size(p) << '\n';
        fs::remove(p);
        try {
            fs::file_size("/dev"); // attempt to get size of a directory
        }
        catch (fs::filesystem_error& e) {
            std::cout << e.what() << '\n';
        }*/

    }

    void CreateDirectory()
    {
        std::filesystem::create_directories("R:\\Temp\\FILES\\TestDirs\\a\\b");
        std::vector<std::filesystem::path> paths = {
                "R:\\Temp\\FILES\\TestDirs\\a\\b\\1.txt",
                "R:\\Temp\\FILES\\TestDirs\\a\\b\\2.txt",
                "R:\\Temp\\FILES\\TestDirs\\a\\3.dat",
                "R:\\Temp\\FILES\\TestDirs\\4.dat"
        };
        for (const auto& filepath : paths) {
            std::cout << filepath.filename() << std::endl;
        }

        {
            std::filesystem::recursive_directory_iterator begin("R:\\Temp\\FILES\\TestDirs");
            std::filesystem::recursive_directory_iterator end;
            std::vector<std::filesystem::path> subdirs;
            std::copy_if(begin, end, std::back_inserter(subdirs), [](const std::filesystem::path& path) {
                return std::filesystem::is_directory(path);
            });

            std::cout << "-- subdirs --" << std::endl;
            std::copy(subdirs.begin(), subdirs.end(), std::ostream_iterator<std::filesystem::path>(std::cout, "\n"));
        }


        {
            std::filesystem::recursive_directory_iterator begin("R:\\Temp\\FILES\\TestDirs");
            std::filesystem::recursive_directory_iterator end;

            // Get file list of extension .txt using algirithm copy_if
            std::vector<std::filesystem::path> txtFiles;
            std::copy_if(begin, end, std::back_inserter(txtFiles), [](const std::filesystem::path& path) {
                return std::filesystem::is_regular_file(path) && (path.extension() == ".txt");
            });
            // List files .
            std::cout << "-- txt files --" << std::endl;
            std::copy(txtFiles.begin(), txtFiles.end(), std::ostream_iterator<std::filesystem::path>(std::cout, "\n"));
        }
    }

    void Is_Dir_Exists() {
        const std::filesystem::path dirPath(R"(S:\Temp\Folder_For_Testing\File_1.txt)");

        // Check is dir exists:
        if (std::filesystem::exists(dirPath)) {
            std::cout << "Directory " << dirPath << " exists" << std::endl;
        }
    }

    void Is_Dir_Exists_ErrrCpde() {
        const std::filesystem::path dirPath(R"(S:\Temp\Folder_For_Testing\File_1.txt)");

        std::error_code errCode;
        if (std::filesystem::exists(dirPath, errCode)) {
            std::cout << "Directory " << dirPath << " exists" << std::endl;
        }

        std::cout << "errCode.value() = " << errCode.value() << std::endl;
        std::cout << "errCode.message() = " << errCode.message() << std::endl;
        // std::cout << "errCode.message() = " << errCode.<< std::endl;
    }

    void CurrentPath() {
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << currentPath << std::endl;
    }


    void GetCurrentDir()
    {
        std::filesystem::path local(".");
        std::cout << "local: " << local << "\n";

        std::filesystem::path from_root = absolute(local);
        std::cout << "absolute: " << from_root << "\n";
    }


    void Hard_Link_Count() {
        std::filesystem::path path = std::filesystem::current_path();
        std::cout << path << std::endl;

        std::cout << "Number of hard links for current path is "
                  << std::filesystem::hard_link_count(path) << std::endl;

        // each ".." is a hard link to the parent directory, so the total number
        // of hard links for any directory is 2 plus number of direct subdirectories
        path = std::filesystem::current_path() / ".."; // each dot-dot is a hard link to parent
        std::cout << path << std::endl;

        std::cout << "Number of hard links for .. is " << std::filesystem::hard_link_count(path) << std::endl;
    }


    //------------------------------------------------------------------------------

    void print_space_info(auto const& dirs, int width = 14)
    {
        std::cout << std::left;
        for (const auto s : { "Capacity", "Free", "Available", "Dir" })
            std::cout << "| " << std::setw(width) << s << ' ';
        std::cout << '\n';
        std::error_code ec;
        for (auto const& dir : dirs) {
            const std::filesystem::space_info si = std::filesystem::space(dir, ec);
            std::cout
                    << "| " << std::setw(width) << static_cast<std::intmax_t>(si.capacity) << ' '
                    << "| " << std::setw(width) << static_cast<std::intmax_t>(si.free) << ' '
                    << "| " << std::setw(width) << static_cast<std::intmax_t>(si.available) << ' '
                    << "| " << dir << '\n';
        }
    }

    void Space__Free_Available() {
        const auto dirs = { "C:\\Temp", "C:\\Users" };
        print_space_info(dirs);
    }
};

namespace Filesystem::DocumentStorageTests {

    class DocumentStorage {
    private:
        /** WebApps directory files in-memory storage: **/
        std::unordered_map<std::string, std::string> documents;

        /** WebApps director full path. **/
        std::string webAppsDir;

    public:
        DocumentStorage(const std::string& path = "") {
            webAppsDir = path;
        }

    public:
        std::optional<std::unordered_map<std::string, std::string>::const_iterator>
        GetResource(const std::string& path) const noexcept {
            std::optional<std::unordered_map<std::string, std::string>::const_iterator> result;
            if (auto iter = documents.find(path); documents.end() != iter)
                result.emplace(iter);
            return result;
        }

        /** **/
        bool ReadWebAppDir() {
            for (const auto& path : std::filesystem::recursive_directory_iterator(this->webAppsDir))
                if (true == path.is_regular_file() && false == ReadFile(path.path()))
                    return false;
            return false;
        }

    protected:
        /** **/
        bool ReadFile(const std::filesystem::path& path) {
            try {
                std::string relative_path(path.string().substr(this->webAppsDir.size(), path.string().length() - this->webAppsDir.size()));
                auto inserted = this->documents.emplace(relative_path, "");

                // std::cout << "Readinf file " << relative_path << std::endl;

                /**/
                std::ifstream input_file_stream(path);
                inserted.first->second.reserve(std::filesystem::file_size(path));

                input_file_stream.unsetf(std::ios::skipws);
                std::copy(std::istream_iterator<char>(input_file_stream),
                          std::istream_iterator<char>(),
                          std::back_inserter(inserted.first->second));
                input_file_stream.close();
            }
            catch (const std::exception& exc)
            {
                std::cout << "ERROR:" << exc.what() << std::endl;
            }
            return true;
        }
    };

    /////////////////////////////////

    void Test() {
        DocumentStorage documentStorage("R:\\Projects\\Html");
        documentStorage.ReadWebAppDir();
    }
}

namespace Filesystem::Files {
    namespace fs = std::filesystem;

    /* Test directory: */
    static const std::string testDir = R"(S:\Temp\Folder_For_Testing)";

    void Check_IsFile_Exists() {
        const std::filesystem::path path { R"(/home/andtokm/tmp/TEST_FILES/TestFile.txt)" };
        std::cout << "exists  = " << std::boolalpha <<  std::filesystem::exists(path) << std::endl;
        std::cout << "Is file = " << std::boolalpha <<  std::filesystem::is_regular_file(path) << std::endl;

        const std::filesystem::path parentDir {path.parent_path()};
        std::cout << "Parent = " << parentDir << std::endl;

        std::cout << "Parent exists  = " << std::boolalpha <<  std::filesystem::exists(parentDir) << std::endl;
        std::cout << "Is Parent dir = " << std::boolalpha <<  std::filesystem::is_directory(parentDir) << std::endl;

    }

    void CrateFile() {
        auto testFile = std::filesystem::path(testDir + R"(\Read_Write_Files\File_to_Create.txt)");
        std::ofstream dataFile(testFile);
        if (!dataFile) {
            std::cerr << "OOPS, can't open \"" << testFile.string() << "\"\n";
            std::exit(EXIT_FAILURE); // exit program with failure
        }

        // Write to file:
        dataFile << "Hello!";
    }

    void CopyFile() {
        auto src = fs::path(R"(/home/andtokm/tmp/folder_for_testing/file1.txt)");
        auto dst = fs::path(R"(/home/andtokm/tmp/folder_for_testing/file1_1.txt)");

        try {
            fs::copy_file(src, dst);
            std::cout << dst << ". Created: " << std::boolalpha << fs::exists(dst) << std::endl;
        }
        catch (std::exception& exc) {
            std::cout << exc.what() << std::endl;
        }
    }

    void MoveFile() {
        const auto src = std::filesystem::path(R"(/home/andtokm/tmp/folder_for_testing/file1.txt)");
        const auto dst = std::filesystem::path(R"(/home/andtokm/tmp/folder_for_testing/dst/file1_1.txt)");

        std::filesystem::rename(src, dst);
    }


    void Last_Write_Time() {
        using namespace std::chrono_literals;
        const auto file = fs::path(testDir + R"(\File_1.txt)");

        auto lwt = std::filesystem::last_write_time(file);

        //std::time_t cftime = decltype(lwt)::clock::
        //std::cout << "File write time is " << std::asctime(std::localtime(&cftime)) << '\n';
    }

    void File_Params() {
        const auto file = fs::path(testDir + R"(\File_1.txt)");

        std::cout << "exists: " << std::filesystem::exists(file) << "\n"
                  << "root_name: " << file.root_name() << "\n"
                  << "root_path: " << file.root_path() << "\n"
                  << "relative_path: " << file.relative_path() << "\n"
                  << "parent_path: " << file.parent_path() << "\n"
                  << "filename: " << file.filename() << "\n"
                  << "stem: " << file.stem() << "\n"
                  // << "socket: " << file.is_socket() << "\n"
                  << "extension: " << file.extension() << "\n\n";

        std::cout << "has_extension: " << std::boolalpha << file.has_extension() << "\n"
                  << "has_filename: " << std::boolalpha << file.has_filename() << "\n"
                  << "has_parent_path: " << std::boolalpha << file.has_parent_path() << "\n"
                  << "has_relative_path: " << std::boolalpha << file.has_relative_path() << "\n"
                  << "has_root_directory: " << std::boolalpha << file.has_root_directory() << "\n"
                  << "has_root_name: " << std::boolalpha << file.has_root_name() << "\n"
                  << "has_root_path: " << std::boolalpha << file.has_root_path() << "\n"
                  << "has_stem: " << std::boolalpha << file.has_stem() << "\n";
    }
}

namespace Filesystem::Permissions {

    void demo_perms(std::filesystem::perms p)
    {
        std::cout << ((p & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? "r" : "-")
                  << ((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? "w" : "-")
                  << ((p & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? "x" : "-")
                  << ((p & std::filesystem::perms::group_read) != std::filesystem::perms::none ? "r" : "-")
                  << ((p & std::filesystem::perms::group_write) != std::filesystem::perms::none ? "w" : "-")
                  << ((p & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? "x" : "-")
                  << ((p & std::filesystem::perms::others_read) != std::filesystem::perms::none ? "r" : "-")
                  << ((p & std::filesystem::perms::others_write) != std::filesystem::perms::none ? "w" : "-")
                  << ((p & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? "x" : "-")
                  << '\n';
    }

    void Get_File_Permissions() {
        const std::filesystem::path file = R"(S:\Temp\Folder_For_Testing\Read_Write_Files\read_write.txt)";
        demo_perms(std::filesystem::status(file).permissions());
    }

    void If_File_Writable() {
        {
            const std::filesystem::path file = R"(S:\Temp\Folder_For_Testing\Read_Write_Files\read_write.txt)";
            auto status = std::filesystem::status(file);

            std::cout << "File: " << file << std::endl;
            if ((status.permissions() & std::filesystem::perms{ 0222 }) != std::filesystem::perms::none) {
                std::cout << "File writable" << std::endl;
            }
        }

        {
            const std::filesystem::path file = R"(S:\Temp\Folder_For_Testing\Read_Write_Files\no_writable_file.txt)";
            auto status = std::filesystem::status(file);

            std::cout << "File: " << file << std::endl;
            if ((status.permissions() & std::filesystem::perms{ 0222 }) != std::filesystem::perms::none) {
                std::cout << "File writable" << std::endl;
            }
            else
                std::cout << "File NON writable" << std::endl;
        }

    }
}

namespace Filesystem::Sizes {

    void FileSize() {
        constexpr std::string_view file {R"(S:\Temp\TESTING_ROOT_DIR\test_run.log)"};
        std::cout << "File size = " << std::filesystem::file_size(file) << std::endl;
    }

    void TestFileSize()
    {
        constexpr std::string_view file_path
                { R"(/home/andtokm/DiskS/Temp/TESTING_ROOT_DIR/test_run.log)"};

        if (std::ifstream file(file_path.data(), std::ios::binary); file.is_open() && file.good()) {
            file.seekg(0, std::ios_base::end);
            const size_t fileSize = file.tellg();
            file.seekg(0, std::ios_base::beg);

            std::cout << "Size : " << fileSize << std::endl;
            std::cout << "Size : " << std::filesystem::file_size(file_path) << std::endl;
        }
    }

    size_t dir_size(const std::filesystem::path& path)
    {
        size_t size = 0;
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(path))
            {
                std::cout << entry.path() << std::endl;
                if (std::filesystem::is_regular_file(entry.status()) || std::filesystem::is_symlink(entry.status()))
                {
                    // auto err = std::error_code{};
                    auto filesize = std::filesystem::file_size(entry);
                    if (filesize != static_cast<size_t>(-1))
                        size += filesize;
                }
            }
        }
        return size;
    }


    void Directory_Size() {
        constexpr std::string_view dirPath{ R"(S:\Temp\TESTING_ROOT_DIR)" };
        const size_t size = dir_size(dirPath);
        // std::cout << std::format("Folder '{}' size is {}", dirPath, size) << std::endl;
    }

    void Experiments() {
        constexpr std::string_view path{ R"(S:\Temp\TESTING_ROOT_DIR)" };
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(path))
        {
            std::cout << entry.path() << std::endl;
        }
    }
}

namespace Filesystem::Tests {

    bool ContainsFiles(const std::filesystem::path& dir,
                       std::string_view ext) {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (std::filesystem::is_regular_file(entry.status()) &&
                entry.path().extension() == ext) {
                return true;
            }
        }
        return false;
    }

    void GetDirListing()
    {
        constexpr std::string_view dirPath = R"(S:\Projects\cpp\third_party\VTK)";

        std::set<std::filesystem::path> folders;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
            if (std::filesystem::is_directory(entry.status())) {
                folders.insert(entry.path());
            }
        }

        std::for_each(folders.cbegin(), folders.cend(), [](const auto& dir) {
            if (ContainsFiles(dir, ".h"))
                std::cout << dir << std::endl;
        });
    }


    bool isHeaderFile(const std::filesystem::path& p)
    {
        return std::filesystem::is_regular_file(p) && p.extension() == ".h";
    }

    [[nodiscard]]
    std::vector<std::filesystem::path> CollectPaths(const std::filesystem::path& startPath)
    {
        std::vector<std::filesystem::path> paths;
        std::filesystem::recursive_directory_iterator dirpos{ startPath };
        std::copy_if(std::filesystem::begin(dirpos),
                     std::filesystem::end(dirpos), std::back_inserter(paths), isHeaderFile);
        return paths;
    }



    void Tests() {

        constexpr std::string_view dir = R"(S:\Projects\cpp\third_party\VTK)";
        std::vector paths = CollectPaths(dir);
        std::cout << paths.size() << std::endl;

        size_t count {0};
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
            if (std::filesystem::is_regular_file(entry.status()) && isHeaderFile(entry)) {
                ++count;
            }
        }
        std::cout << count << std::endl;

    }
}


namespace Filesystem::Paths {

    void PathTests() {
        auto path = std::filesystem::path{R"(C:\Users\Marius\Documents\sample.file.txt)"};
        std::cout << path << std::endl;

        path.replace_filename("output.txt");
        std::cout << path << std::endl;

        path.replace_extension(".log");
        std::cout << path << std::endl;

        std::filesystem::path pth1 = std::filesystem::path{R"(/home/andtokm/tmp)"};
        pth1.append(path.string());
        std::cout << pth1 << std::endl;
    }

    void GetPathParts() {
        auto path1 = std::filesystem::path{ R"(:/Users/Marius/Documents/sample.file.txt)" };
        for (auto const & part : path1) {
            std::cout << part << std::endl;
        }
    }
}


namespace Filesystem::Attributes {

    struct Attributes {
        size_t size_bytes;
        size_t n_directories;
        size_t n_files;

        Attributes& operator+=(const Attributes& other) {
            this->size_bytes += other.size_bytes;
            this->n_directories += other.n_directories;
            this->n_files += other.n_files;
            return *this;
        }
    };

    Attributes explore(const std::filesystem::directory_entry& directory) {
        Attributes attributes {};
        for (const auto &entry: std::filesystem::recursive_directory_iterator{directory.path()}) {
            if (entry.is_directory()) {
                attributes.n_directories++;
            } else {
                attributes.n_files++;
                attributes.size_bytes += entry.file_size();
            }
        }
        return attributes;
    }

    void print_line(const Attributes& attributes, std::string_view path) {
        std::cout << std::setw(14) << attributes.size_bytes
                  << std::setw(7) << attributes.n_files
                  << std::setw(7) << attributes.n_directories
                  << " " << path << "\n";
    }

    void Test()
    {
        constexpr std:: string_view path {R"(/home/andtokm/tmp/folder_for_testing)"};

        Attributes root_attributes{};
        for (const auto& entry : std::filesystem::directory_iterator { path }) {
            try {
                if (entry.is_directory()) {
                    const auto attributes = explore(entry);
                    root_attributes += attributes;
                    print_line(attributes, entry.path().string());
                    root_attributes.n_directories++;
                } else {
                    root_attributes.n_files++;
                    std::error_code ec {};
                    root_attributes.size_bytes += entry.file_size(ec);
                    if (ec) {
                        std::cout << "Error reading file size: " << entry.path().string() << std::endl;
                    }
                }
            } catch(const std::exception& exc) {
                std::cout << "Opps!\n";
            }
        }

        print_line(root_attributes, path);
    }
}

namespace Filesystem::Experiments {

    void CreateFile_and_GetTime() {

        const auto path = std::filesystem::path{ R"(/home/andtokm/tmp/folder_for_testing/file3.txt)" };
        /*if (const std::ofstream file (path); !file.is_open() || !file.good()) {
            std::cout << "Failed to create/open file " << path << std::endl;
        }*/

        auto filetime = std::filesystem::last_write_time(path);
        const auto toNow = std::filesystem::file_time_type::clock::now() - filetime;
        const auto elapsedSec = duration_cast<std::chrono::seconds>(toNow).count();
        std::cout << "File has been created " << elapsedSec << " seconds ago\n";

        /*
        auto filetime = std::filesystem::last_write_time(path);
        std::time_t convfiletime = std::chrono::system_clock::to_time_t(filetime);
        std::cout << "Updated: " << std::ctime(&convfiletime) << '\n';
        */
    }

    void VARIOUS_TESTS()
    {
        constexpr std::string_view filePath {R"(/home/andtokm/tmp)"};
        std::filesystem::path path {filePath};

        std::cout << '\n' << path << std::endl;
        std::cout << "   exists: " << std::boolalpha << std::filesystem::exists(path) << std::endl;
        std::cout << "   has_extension: " << path.has_extension() << std::endl;
        std::cout << "   has_filename: " << path.has_filename() << std::endl<< std::endl;
        // std::cout << "has_filename: " << path.ha() << std::endl;

        auto pathCopy {path};
        pathCopy.remove_filename();

        std::cout << pathCopy << std::endl;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(pathCopy)) {
            std::cout << "   " << entry.path() << std::endl;
            std::cout << "       regular: " << entry.is_regular_file()
                      << ", directory: "  << entry.is_directory()
                      << ", fifo: "  << entry.is_fifo()
                      << ", other: "  << entry.is_other()
                      << std::endl;

        }
    }

    void ListDirectory_GetModifiedTime()
    {
        constexpr std::string_view dirPath {R"(/home/andtokm/tmp)"};
        for (const auto& entry : std::filesystem::directory_iterator(dirPath))
        {
            const std::string& filename = entry.path().filename().string();
            std::filesystem::file_time_type writeTime  = std::filesystem::last_write_time(entry);

            const auto toNow = std::filesystem::file_time_type::clock::now() - writeTime;
            const auto elapsedSec = duration_cast<std::chrono::seconds>(toNow).count();

            if (entry.is_directory()) {
                std::cout << filename << "   " << elapsedSec << std::endl;
            }
            else if (entry.is_regular_file()) {
                std::cout << filename << ", size " << std::filesystem::file_size(entry) << " bytes, "
                          << "   " << elapsedSec << std::endl;
            }
            else
                std::cout << filename<< '\n';

        }
    }
};


void Filesystem::TestAll()
{

    // TempDirectoryPath();
    // StandartMethods();
    // StandartMethods2();

    // DirectoryIteratorTest();
    // DirectoryIteratorTest_Recursive();
    // DirectoryIteratorTest_Recursive_WithExtension();
    // Enumerate_Directory();

    // CreateDirectory();
    // CreateDirectoryTest();
    // DeleteDirectoryTest();
    // CopyDirTest();

    // Is_Dir_Exists();
    // Is_Dir_Exists_ErrrCpde();

    // CurrentPath();
    // GetCurrentDir();

    // Hard_Link_Count();

    // Space__Free_Available();

    // Files::File_Params();
    // Files::CrateFile();
    // Files::CopyFile();
    // Files::MoveFile();
    // Files::Last_Write_Time();
    // Files::Check_IsFile_Exists();


    // Permissions::Get_File_Permissions();
    // Permissions::If_File_Writable();

    // DocumentStorageTests::Test();


    Sizes::FileSize();
    Sizes::TestFileSize();
    // Sizes::Directory_Size();
    // Sizes::Experiments();

    // Tests::GetDirListing();
    // Tests::Tests();

    // Paths::PathTests();
    // Paths::GetPathParts();

    // Experiments::CreateFile_and_GetTime();
    // Experiments::VARIOUS_TESTS();
    // Experiments::ListDirectory_GetModifiedTime();


    // Attributes::Test();
}



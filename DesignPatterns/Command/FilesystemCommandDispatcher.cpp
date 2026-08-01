/**============================================================================
Name        : FilesystemCommandDispatcher.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FilesystemCommandDispatcher.cpp
============================================================================**/

#include "Command.hpp"

#include <fstream>
#include <filesystem>
#include <variant>
#include <print>

namespace
{
    struct CreateFile
    {
        std::filesystem::path path;
    };

    struct DeleteFile
    {
        std::filesystem::path path;
    };

    struct RenameFile
    {
        std::filesystem::path oldPath;
        std::filesystem::path newPath;
    };

    using CommandType = std::variant<CreateFile,DeleteFile,RenameFile>;

    class FileSystemService
    {
    public:
        void execute(const CreateFile& cmd)
        {
            // std::ofstream file(cmd.path);
            std::println("CreateFile({})", cmd.path.string());
        }

        void execute(const DeleteFile& cmd)
        {
            // std::filesystem::remove(cmd.path);
        }

        void execute(const RenameFile& cmd)
        {
            // std::filesystem::rename(cmd.oldPath,cmd.newPath);
            std::println("RenameFile({} - {})", cmd.oldPath.string(), cmd.newPath.string());
        }
    };

    class CommandDispatcher
    {
    public:
        explicit CommandDispatcher(FileSystemService& fs): fSystem { fs } {
        }

        void dispatch(const CommandType& cmd)
        {
            std::visit([this](auto const& c){
                fSystem.execute(c);
            },cmd);
        }

    private:

        FileSystemService& fSystem;
    };
}


void command::filesystem_command_dispatcher::TestAll()
{
    FileSystemService fs;
    CommandDispatcher dispatcher(fs);

    dispatcher.dispatch(CreateFile{"report.txt"});
    dispatcher.dispatch(RenameFile{"report.txt","report_v2.txt"});

    // CreateFile(report.txt)
    // RenameFile(report.txt - report_v2.txt)
}

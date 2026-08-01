/**============================================================================
Name        : CommandBusHandler_Crtp.cpp.cpp
Created on  : 01.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CommandBusHandler_Crtp.cpp.cpp
============================================================================**/

#include "Command.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace
{
    struct CreateUserCommand
    {
        std::string login;
        std::string email;
    };

    struct DeleteUserCommand
    {
        uint64_t id;
    };

    struct RenameUserCommand
    {
        uint64_t id;
        std::string login;
    };

    struct UserRepository
    {
        void createUser(const std::string& login, const std::string& email){
            std::cout << "Create user: " << login << " <" << email << ">\n";
        }

        void deleteUser(const uint64_t id){
            std::cout << "Delete user #" << id << '\n';
        }

        void renameUser(const uint64_t id, const std::string& login) {
            std::cout << "Rename user #" << id << " -> " << login << '\n';
        }
    };

    template<typename Command, typename Handler>
    struct BaseHandler
    {
        explicit BaseHandler(UserRepository& repository) noexcept : repository { repository } {
        }

        void handle(const Command& command) const{
            static_cast<const Handler&>(*this).handle(command);
        }

    protected:

        UserRepository& repository;
    };

    struct CreateUserHandler: BaseHandler<CreateUserCommand, CreateUserHandler>
    {
        using BaseHandler::BaseHandler;

        void handle(const CreateUserCommand& command) const{
            repository.createUser(command.login, command.email);
        }
    };

    struct DeleteUserHandler: BaseHandler<DeleteUserCommand, DeleteUserHandler>
    {
        using BaseHandler::BaseHandler;

        void handle(const DeleteUserCommand& command) const{
            repository.deleteUser(command.id);
        }
    };

    struct RenameUserHandler: BaseHandler<RenameUserCommand, RenameUserHandler>
    {
        using BaseHandler::BaseHandler;

        void handle(const RenameUserCommand& command) const{
            repository.renameUser(command.id, command.login);
        }
    };

    class CommandBus
    {
        using HandlerFunction = std::function<void(const void*)>;

    public:

        template<typename Command, typename Handler>
        void registerHandler(Handler& handler)
        {
            handlers_[typeid(Command)] = [&handler](const void* command){
                handler.handle(*static_cast<const Command*>(command));
            };
        }

        template<typename Command>
        void dispatch(const Command& command) const
        {
            const auto it = handlers_.find(typeid(Command));
            if (it == handlers_.end()) {
                throw std::runtime_error("Command handler is not registered.");
            }
            it->second(&command);
        }

    private:

        std::unordered_map<std::type_index, HandlerFunction> handlers_;
    };

}

void command::command_bus_handler_crtp::TestAll()
{
    UserRepository repository;

    CreateUserHandler createUserHandler(repository);
    DeleteUserHandler deleteUserHandler(repository);
    RenameUserHandler renameUserHandler(repository);

    CommandBus commandBus;

    commandBus.registerHandler<CreateUserCommand>(createUserHandler);
    commandBus.registerHandler<DeleteUserCommand>(deleteUserHandler);
    commandBus.registerHandler<RenameUserCommand>(renameUserHandler);

    commandBus.dispatch(CreateUserCommand{
        .login = "Alice",
        .email = "alice@example.com"
    });

    commandBus.dispatch(RenameUserCommand{
        .id = 1,
        .login = "Alice Smith"
    });

    commandBus.dispatch(DeleteUserCommand{
        .id = 1
    });

    /**
    Create user: Alice <alice@example.com>
    Rename user #1 -> Alice Smith
    Delete user #1
    **/
}
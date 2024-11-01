/**============================================================================
Name        : ChatMediator.cpp
Created on  : 07.01.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Chat room Mediator pattern example
============================================================================**/

#include "Mediator.h"

#include <iostream>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <memory>

namespace Mediator::Chat
{
    using UserID = uint64_t;

    struct IChatRoom
    {
        virtual void sendMessage(const std::string& message, size_t userId) = 0;
        virtual void addUser(const std::shared_ptr<struct IUser>& user) = 0;

        virtual ~IChatRoom() = default;
    };

    struct IUser
    {
        virtual void send(const std::string& message, size_t userId) = 0;
        virtual void receive(const std::string& message) = 0;

        virtual void setRoom(const std::shared_ptr<IChatRoom>& room) = 0;
        virtual void userJoined(const std::shared_ptr<IUser>& user) = 0;

        [[nodiscard]]
        virtual std::string getName() const noexcept = 0;

        [[nodiscard]]
        virtual size_t getID() const noexcept = 0;

        virtual ~IUser() = default;
    };


    class User: public IUser
    {
    public:
        std::shared_ptr<IChatRoom> mediator { nullptr };
        UserID id {0};
        std::string name {};

    public:
        User(const UserID id, std::string name): id { id }, name { std::move(name) } {
            // Do something
        }

        User(std::shared_ptr<IChatRoom> room, const UserID id, std::string name):
                mediator { std::move(room) }, id { id }, name { std::move(name) } {
            // Do something
        }

        void setRoom(const std::shared_ptr<IChatRoom>& room) override {
            this->mediator = room;
        }

        void userJoined(const std::shared_ptr<IUser>& user) override {
            std::cout << "\tUser '" << name << "' informed that '" << user->getName()
                      << "' has joined the room\n";
        }

        [[nodiscard]]
        std::string getName() const noexcept override {
            return name;
        }

        [[nodiscard]]
        UserID getID() const noexcept override {
            return id;
        }
    };

    class ChatRoom final : public IChatRoom,
                           public std::enable_shared_from_this<ChatRoom>
    {
        std::unordered_map<size_t, std::weak_ptr<IUser>> usersMap {};

    public:
        void sendMessage(const std::string& message,
                         const UserID userId) override {
            if (const auto userIter = usersMap.find(userId); usersMap.end() != userIter) {
                if (const std::shared_ptr<IUser> user = userIter->second.lock(); user) {
                    user->receive(message);
                }
            }
        }

        void addUser(const std::shared_ptr<IUser>& user) override
        {
            std::cout << "'" << user->getName() << "' has entered the chat room\n";

            usersMap.emplace(user->getID(), user);
            user->setRoom(this->shared_from_this());

            for (const auto& [userID, userPtrWeak]: usersMap)
            {
                if (const std::shared_ptr<IUser>& exisingUser = userPtrWeak.lock(); exisingUser && user->getID() != userID)
                {
                    exisingUser->userJoined(user);
                }
            }
        }
    };

    class ChatUser final : public User
    {
    public:
        [[maybe_unused]]
        ChatUser(std::shared_ptr<IChatRoom> room, const UserID id, std::string name):
                User { std::move(room), id, std::move(name) } {
        }

        ChatUser(const UserID id, std::string name):
                User {nullptr, id, std::move(name)} {
        }

        void send(const std::string& message,
                  const UserID userId) override {
            std::cout << name << " ==> Sending message: " << message << std::endl;
            mediator->sendMessage(message, userId);
        }

        void receive(const std::string& message) override {
            std::cout << name << " <== Received Message: " << message << std::endl;
        }
    };
}

void CharRoomMediator()
{
    using namespace Mediator::Chat;

    auto chatroom = std::make_shared<ChatRoom>();

    auto alex = std::make_shared<ChatUser>(1, "Alex");
    auto brian = std::make_shared<ChatUser>(2, "Brian");
    auto charles = std::make_shared<ChatUser>(3, "Charles");
    auto david = std::make_shared<ChatUser>(4, "David");

    chatroom->addUser(alex);
    chatroom->addUser(brian);
    chatroom->addUser(charles);
    chatroom->addUser(david);

    std::cout << std::endl;

    alex->send("Hello brian", 2);
    brian->send("Hey buddy", 1);
}
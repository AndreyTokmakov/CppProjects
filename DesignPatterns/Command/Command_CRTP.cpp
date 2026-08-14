/**============================================================================
Name        : Command_CRTP.cpp
Created on  : 22.06.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Command_CRTP
============================================================================**/

#include "Command.hpp"

#include <iostream>
#include <string>
#include <functional>

namespace
{
    template<typename Derived>
    struct ICommand
    {
        void execute()
        {
            static_cast<Derived*>(this)->execute();
        }
    };

    struct Player
    {
        int x { 0 };
        int y { 0 };

        void move(const int dx, const int dy)
        {
            x += dx;
            y += dy;
            std::cout << "move { x: " << x << " y: " << y << "}\n";
        }

        void jump()
        {
            std::cout << "jump!!\n";
        }
    };

    struct MoveCommand : public ICommand<Player>
    {
        Player& player ;
        int x { 0 };
        int y { 0 };

        MoveCommand(Player& player, const int dx, const int dy):
            player { player }, x { dx }, y { dy } {
        }

        void execute() {
            player.move(x, y);
        }
    };

    struct JumpCommand : public ICommand<Player>
    {
        Player& player ;

        explicit JumpCommand(Player& player):player { player } {
        }

        void execute() const {
            player.jump();
        }
    };

    struct StaticCommand
    {
        static constexpr size_t StorageSize { 32 };

        // StaticCommand() = default;

        template<typename Cmd>
        explicit StaticCommand(Cmd cmd)
        {
            static_assert(sizeof(Cmd) <= StorageSize, "Command is to large");
            static_assert(std::is_trivially_destructible_v<Cmd>, "Command is not trivially destructible");

            new (storage.data()) Cmd { std::move(cmd) };
            callback = [](void *ptr) {
                static_cast<Cmd*>(ptr)->execute();
            };
        }

        void execute()
        {
            std::invoke(callback, storage.data());
            // callback(storage.data());
        }

    private:

        alignas(std::max_align_t) std::array<uint8_t, StorageSize> storage {};
        void (*callback) (void*) { nullptr };

    };
}

void command::command_crtp::TestAll()
{
    Player player;

    std::vector<StaticCommand> commands;
    commands.emplace_back(MoveCommand { player, 5, 3 });
    commands.emplace_back(JumpCommand { player });
    commands.emplace_back(MoveCommand { player, -2, 1 });

    for ( auto& command : commands) {
        command.execute();
    }
}



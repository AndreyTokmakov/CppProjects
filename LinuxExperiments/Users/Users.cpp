//============================================================================
// Name        : Users.cpp
// Created on  : 20.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Users
//============================================================================

#include <iostream>
#include <string_view>
#include <crypt.h>

#include "Users.h"
#include "../common.h"

namespace Users
{
    void GetUserID() {
        std::cout << "Real User ID = " << getuid() << '\n'
                  << "Effective User ID = " << geteuid() << std::endl;
    }

    void checkRunningUnder()
    {
        const uint32_t userID { getuid() };
        if (0 == userID) {
            std::cout << "Root" << std::endl;
        }
        else {
            std::cout << "NOT Root" << std::endl;
        }
    }

    void GetUserInfo_ByID() {
        const uid_t userid = getuid();
        std::cout << "Real User ID = " << userid<< std::endl;

        const passwd *userinfo = getpwuid(userid);
        if (nullptr != userinfo){
            std::cout << "user name    : " << userinfo->pw_name << '\n'
                      << "user home dir: " << userinfo->pw_dir << '\n'
                      << "user shell   : " << userinfo->pw_shell << std::endl;
        };
    }

    void GetUserInfo_ByName() {
        constexpr std::string_view userName { "andtokm" };

        const passwd *userinfo = getpwnam(userName.data());
        if (nullptr != userinfo){
            std::cout << "user name     : " << userinfo->pw_name << '\n'
                      << "user uid      : " << userinfo->pw_uid << '\n'
                      << "user group    : " << userinfo->pw_gid << '\n'
                      << "user home dir : " << userinfo->pw_dir << '\n'
                      << "user pass     : " << userinfo->pw_passwd << '\n'
                      << "user shell    : " << userinfo->pw_shell << std::endl;
        };
    }

    void EnumerateUsers() {
        passwd *userinfo {nullptr};
        while ((userinfo = getpwent())) {
            std::cout << userinfo->pw_name << " | "
                      << userinfo->pw_uid << " | "
                      << userinfo->pw_gid << " | "
                      << userinfo->pw_dir << " | "
                      << userinfo->pw_shell << std::endl;
        }

        endpwent();
    }

    void Enscrypt_Password() {
        constexpr std::string_view password {"qwerty"};
        constexpr std::string_view salt {"12345"};

        const char* result = crypt(password.data(), salt.data());
        std::cout << password << " + " << salt << " ===> " << result << std::endl;
    }
};


// TODO: Add read from file /etc/passwd
void Users::TestAll()
{
    // GetUserID();
    checkRunningUnder();

    // GetUserInfo_ByID();

    // GetUserInfo_ByName();

    // EnumerateUsers();

    // Enscrypt_Password();

};

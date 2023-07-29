/**============================================================================
Name        : SshExperiments.cpp
Created on  : 25.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SshExperiments
============================================================================**/

#include "SshExperiments.h"

#include "libssh2_config.h"
#include <libssh2.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <sys/types.h>
#include <cstdio>

#include <iostream>
#include <string_view>
#include <memory>

namespace SshExperiments
{
    int waitSocket(int socket_fd,
                   LIBSSH2_SESSION *session)
    {
        fd_set fd, *writefd = nullptr, *readfd = nullptr;

        FD_ZERO(&fd);
        FD_SET(socket_fd, &fd);

        /* now make sure we wait in the correct direction */
        const int dir = libssh2_session_block_directions(session);
        if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
            readfd = &fd;

        if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
            writefd = &fd;

        timeval timeout {.tv_sec = 10, .tv_usec = 0};
        return select(socket_fd + 1, readfd, writefd, nullptr, &timeout);;
    }

    void closeChannel(LIBSSH2_CHANNEL *channel)
    {
        libssh2_channel_free(channel);
        channel = nullptr;
    }

    void shutdown(LIBSSH2_SESSION *session,
                  const int socket)
    {
        // libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        // libssh2_session_free(session);

        if (-1 == close(socket)) {
            std::cerr << "Failed to close socket. Error = " << socket << std::endl;
        };
        libssh2_exit();
    }

    void closeSocketAndExit(const int socket)
    {
        if (-1 == close(socket)) {
            std::cerr << "Failed to close socket. Error = " << socket << std::endl;
        };
        libssh2_exit();
    }

    void execSshCommand_ORIG()
    {
        constexpr std::string_view hostname { "192.168.101.1" };
        constexpr std::string_view commandline { "duf" };
        constexpr std::string_view username { "andtokm" };
        constexpr std::string_view password { "123!@#QWEqwe" };
        constexpr uint16_t sshPort {22};

        int rc = libssh2_init(0);
        if (rc != 0) {
            fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
            return;
        }

        /* Ultra basic "connect to port 22 on localhost"
         * Your code is responsible for creating the socket establishing the connection
         */
        const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
        // TODO: Check socket

        const sockaddr_in server {PF_INET, htons(sshPort),
                                  {.s_addr = inet_addr(hostname.data())}, {}};
        if (0 != ::connect(sock, (sockaddr*)&server, sizeof(server))) {
            fprintf(stderr, "failed to connect!\n");
            return;
        }

        /* Create a session instance */
        LIBSSH2_SESSION *session = libssh2_session_init();
        if (!session)
            return;

        /* tell libssh2 we want it all done non-blocking */
        libssh2_session_set_blocking(session, 0);

        // start it up. This will trade welcome banners, exchange keys, and setup crypto, compression, and MAC layers
        while ((rc = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
            return;
        }

        LIBSSH2_KNOWNHOSTS *nh = libssh2_knownhost_init(session);
        if(!nh) { /* eeek, do cleanup here */
            return;
        }

        /* read all hosts from here */
        libssh2_knownhost_readfile(nh, "known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

        /* store all known hosts to here */
        libssh2_knownhost_writefile(nh, "dumpfile",LIBSSH2_KNOWNHOST_FILE_OPENSSH);

        size_t len {0};
        int type {0};
        const char *fingerprint = libssh2_session_hostkey(session, &len, &type);

        if (fingerprint) {
            libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
            /* introduced in 1.2.6 */
            int check = libssh2_knownhost_checkp(nh, hostname.data(), 22,
                                                 fingerprint, len,
                                                 LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                                 LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                 &host);
#else
            /* 1.2.5 or older */
        int check = libssh2_knownhost_check(nh, hostname,
                                            fingerprint, len,
                                            LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                            LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                            &host);
#endif
            fprintf(stderr, "Host check: %d, key: %s\n", check,
                    (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                    host->key:"<none>");
            /** At this point, we could verify that 'check' tells us the key is fine or bail out. **/
        }
        else { /* eeek, do cleanup here */
            return;
        }
        libssh2_knownhost_free(nh);


        if (!password.empty()) {
            /* We could authenticate via password */
            while((rc = libssh2_userauth_password(session, username.data(), password.data())) == LIBSSH2_ERROR_EAGAIN);
            if (rc) {
                fprintf(stderr, "Authentication by password failed.\n");
                shutdown(session, sock);
                return;
            }
        }
        else { /* Or by public key */
            while((rc = libssh2_userauth_publickey_fromfile(session,
                                                            username.data(),
                                                            "/home/user/"
                                                            ".ssh/id_rsa.pub",
                                                            "/home/user/"
                                                            ".ssh/id_rsa",
                                                            password.data())) == LIBSSH2_ERROR_EAGAIN);
            if(rc) {
                fprintf(stderr, "\tAuthentication by public key failed\n");
                shutdown(session, sock);
                return;
            }
        }

#if 0
        libssh2_trace(session, ~0);
#endif

        /* Exec non-blocking on the remove host */
        LIBSSH2_CHANNEL *channel {nullptr};
        while ((channel = libssh2_channel_open_session(session)) == nullptr &&
               libssh2_session_last_error(session, nullptr, nullptr, 0) == LIBSSH2_ERROR_EAGAIN) {
            waitSocket(sock, session);
        }
        if (channel == nullptr) {
            fprintf(stderr, "Error\n");
            return;
        }

        while ((rc = libssh2_channel_exec(channel, commandline.data())) == LIBSSH2_ERROR_EAGAIN) {
            waitSocket(sock, session);
        }
        if(rc != 0) {
            fprintf(stderr, "Error\n");
            return;
        }

        int byteCount = 0;
        while (true) {
            /* loop until we block */
            int res = 0;
            do {
                char buffer[0x4000]; // FIXME
                res = libssh2_channel_read(channel, buffer, sizeof(buffer) );

                if (res > 0) {
                    int i;
                    byteCount += res;
                    fprintf(stderr, "We read:\n");
                    for(i = 0; i < res; ++i)
                        fputc(buffer[i], stderr);
                    fprintf(stderr, "\n");
                }
                else {
                    if (res != LIBSSH2_ERROR_EAGAIN) { // no need to output this for the EAGAIN case
                        // std::cerr << "libssh2_channel_read() returned " << res << std::endl;
                    }
                }
            }
            while (res > 0);

            /* this is due to blocking that would occur otherwise, so we loop on this condition */
            if (res == LIBSSH2_ERROR_EAGAIN) {
                waitSocket(sock, session);
            }
            else
                break;
        }

        while((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN)
            waitSocket(sock, session);

        char *exitSignal = (char*)"none";
        int exitcode = 127;
        if (rc == 0) {
            exitcode = libssh2_channel_get_exit_status(channel);
            libssh2_channel_get_exit_signal(channel,
                                            &exitSignal,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr);
        }

        if (exitSignal)
            fprintf(stderr, "\nGot signal: %s\n", exitSignal);
        else {
            std::cout << "Exit: Bytes read: " << byteCount << ", code: " << exitcode << std::endl;
        }

        closeChannel(channel);
        shutdown(session, sock);
    }

    void execSshCommand()
    {
        constexpr std::string_view hostname { "192.168.101.1" };
        constexpr std::string_view commandline { "duf" };
        constexpr std::string_view username { "andtokm" };
        constexpr std::string_view password { "123!@#QWEqwe" };
        constexpr uint16_t sshPort {22};

        int rc = libssh2_init(0);
        if (rc != 0) {
            fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
            return;
        }

        /* Ultra basic "connect to port 22 on localhost"
         * Your code is responsible for creating the socket establishing the connection
         */
        const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
        // TODO: Check socket

        const sockaddr_in server {PF_INET, htons(sshPort),
                                  {.s_addr = inet_addr(hostname.data())}, {}};
        if (0 != ::connect(sock, (sockaddr*)&server, sizeof(server))) {
            fprintf(stderr, "failed to connect!\n");
            return;
        }

        auto closeSession = [](LIBSSH2_SESSION *session) {
            libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
            libssh2_session_free(session);
        };

        /* Create a session instance */
        std::unique_ptr<LIBSSH2_SESSION, decltype(closeSession)> session {libssh2_session_init(), closeSession };
        if (!session)
            return;

        /* tell libssh2 we want it all done non-blocking */
        libssh2_session_set_blocking(session.get(), 0);

        // start it up. This will trade welcome banners, exchange keys, and setup crypto, compression, and MAC layers
        while ((rc = libssh2_session_handshake(session.get(), sock)) == LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
            return;
        }

        {
            std::unique_ptr<LIBSSH2_KNOWNHOSTS, decltype(&libssh2_knownhost_free)> knownHosts {
                    libssh2_knownhost_init(session.get()), libssh2_knownhost_free
            };
            if (!knownHosts)
                return;

            /* read all hosts from here */
            libssh2_knownhost_readfile(knownHosts.get(), "known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

            /* store all known hosts to here */
            libssh2_knownhost_writefile(knownHosts.get(), "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

            size_t len{0};
            int type{0};
            const char *fingerprint = libssh2_session_hostkey(session.get(), &len, &type);

            if (fingerprint)
            {
                libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
                /* introduced in 1.2.6 */
                int check = libssh2_knownhost_checkp(knownHosts.get(), hostname.data(), 22,
                                                     fingerprint, len,
                                                     LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                     &host);
#else       /* 1.2.5 or older */
            int check = libssh2_knownhost_check(knownHosts.get(), hostname.data(),
                                                fingerprint, len,
                                                LIBSSH2_KNOWNHOST_TYPE_PLAIN| LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                &host);
#endif
                fprintf(stderr, "Host check: %d, key: %s\n", check,
                        (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
                        host->key : "<none>");
                /** At this point, we could verify that 'check' tells us the key is fine or bail out. **/
            } else { /* eeek, do cleanup here */
                return;
            }
        }

        if (!password.empty()) {
            /* We could authenticate via password */
            while((rc = libssh2_userauth_password(session.get(), username.data(), password.data())) == LIBSSH2_ERROR_EAGAIN);
            if (rc) {
                fprintf(stderr, "Authentication by password failed.\n");
                closeSocketAndExit(sock);
                return;
            }
        }
        else { /* Or by public key */
            while((rc = libssh2_userauth_publickey_fromfile(session.get(),
                                                            username.data(),
                                                            "/home/user/"
                                                            ".ssh/id_rsa.pub",
                                                            "/home/user/"
                                                            ".ssh/id_rsa",
                                                            password.data())) == LIBSSH2_ERROR_EAGAIN);
            if(rc) {
                fprintf(stderr, "\tAuthentication by public key failed\n");
                shutdown(session.get(), sock);
                return;
            }
        }

#if 0
        libssh2_trace(session, ~0);
#endif

        /* Exec non-blocking on the remove host */
        std::unique_ptr<LIBSSH2_CHANNEL, decltype(&libssh2_channel_free)> channel {
            libssh2_channel_open_session(session.get()), libssh2_channel_free
        };
        while (!channel &&
                libssh2_session_last_error(session.get(), nullptr, nullptr, 0) == LIBSSH2_ERROR_EAGAIN)
        {
            channel.reset( libssh2_channel_open_session(session.get()));
        }

        if (!channel) {
            fprintf(stderr, "Error\n");
            return;
        }

        while ((rc = libssh2_channel_exec(channel.get(), commandline.data())) == LIBSSH2_ERROR_EAGAIN) {
            waitSocket(sock, session.get());
        }
        if(rc != 0) {
            fprintf(stderr, "Error\n");
            return;
        }

        int byteCount = 0;
        while (true) {
            /* loop until we block */
            int res = 0;
            do {
                char buffer[0x4000]; // FIXME
                res = libssh2_channel_read(channel.get(), buffer, sizeof(buffer) );

                if (res > 0) {
                    int i;
                    byteCount += res;
                    fprintf(stderr, "We read:\n");
                    for(i = 0; i < res; ++i)
                        fputc(buffer[i], stderr);
                    fprintf(stderr, "\n");
                }
                else {
                    if (res != LIBSSH2_ERROR_EAGAIN) { // no need to output this for the EAGAIN case
                        // std::cerr << "libssh2_channel_read() returned " << res << std::endl;
                    }
                }
            }
            while (res > 0);

            /* this is due to blocking that would occur otherwise, so we loop on this condition */
            if (res == LIBSSH2_ERROR_EAGAIN) {
                waitSocket(sock, session.get());
            }
            else
                break;
        }

        while ((rc = libssh2_channel_close(channel.get())) == LIBSSH2_ERROR_EAGAIN)
            waitSocket(sock, session.get());

        char *exitSignal = (char*)"none";
        int exitcode = 127;
        if (rc == 0) {
            exitcode = libssh2_channel_get_exit_status(channel.get());
            libssh2_channel_get_exit_signal(channel.get(),
                                            &exitSignal,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr);
        }

        if (exitSignal)
            fprintf(stderr, "\nGot signal: %s\n", exitSignal);
        else {
            std::cout << "Exit: Bytes read: " << byteCount << ", code: " << exitcode << std::endl;
        }

        closeSocketAndExit(sock);
    }
}

void SshExperiments::TestAll([[maybe_unused]] int argc,
                             [[maybe_unused]] char** argv)
{
    // execSshCommand_ORIG();
    execSshCommand();

};

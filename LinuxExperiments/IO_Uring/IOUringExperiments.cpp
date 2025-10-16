/**============================================================================
Name        : IOUringExperiments.cpp
Created on  : 05.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : IOUringExperiments.cpp
============================================================================**/

#include "IOUringExperiments.h"


#include <cassert>
#include <unistd.h>
#include <string>
#include <cstring>

#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace IOUringExperiments::Examples
{
    void simple_example()
    {
        io_uring_params params {};
        memset(&params, 0, sizeof(params));

        /**
         * Создаем инстанс io_uring, не используем никаких кастомных опций.
         * Емкость SQ и CQ буфера указываем как 4096 вхождений.
         */
        io_uring ring {};

        [[maybe_unused]]
        int ret = io_uring_queue_init_params(4, &ring, &params);
        assert(ret == 0);

        constexpr std::string_view message { "hello world!\n"};

        // Добавляем операцию write в очередь SQ.
        io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_write(sqe, STDOUT_FILENO, message.data(), message.size(), 0);

        // Сообщаем io_uring о новых SQE в SQ.
        io_uring_submit(&ring);

        // Ждем пока в CQ появится новое CQE.
        io_uring_cqe *cqe {};
        ret = io_uring_wait_cqe(&ring, &cqe);

        assert(ret == 0);
        assert(cqe->res > 0);  // Проверяем отсутствие ошибок.

        // Dequeue из очереди CQ.
        io_uring_cqe_seen(&ring, cqe);
        io_uring_queue_exit(&ring);
    }
}

namespace IOUringExperiments::Echo_Server
{
#define MAX_CONNECTIONS 4096
#define BACKLOG 512
#define MAX_MESSAGE_LEN 2048
#define IORING_FEAT_FAST_POLL (1U << 5)

    enum class Type {
        NONE,
        Accept,
        Read,
        Write,
    };

    struct ConnInfo
    {
        int fd { -1 };
        Type type { Type::NONE };
    };

    // Буфер для соединений.
    ConnInfo conns[MAX_CONNECTIONS];

    // Для каждого возможного соединения инициализируем буфер для чтения/записи.
    char bufs[MAX_CONNECTIONS][MAX_MESSAGE_LEN];

    void add_accept(io_uring *ring, int fd,
                    sockaddr *client_addr,
                    socklen_t *client_len)
    {
        // Получаем указатель на первый доступный SQE.
        io_uring_sqe *sqe = io_uring_get_sqe(ring);

        // Хелпер io_uring_prep_accept помещает в SQE операцию ACCEPT.
        io_uring_prep_accept(sqe, fd, client_addr, client_len, 0);

        // Устанавливаем состояние серверного сокета в ACCEPT.
        ConnInfo *conn_i = &conns[fd];
        conn_i->fd = fd;
        conn_i->type = Type::Accept;

        // Устанавливаем в поле user_data указатель на socketInfo соответствующий серверному сокету.
        io_uring_sqe_set_data(sqe, conn_i);
    }

    void add_socket_read(io_uring *ring,
                         int fd,
                         size_t size)
    {
        // Получаем указатель на первый доступный SQE.
        io_uring_sqe *sqe = io_uring_get_sqe(ring);

        // Хелпер io_uring_prep_recv помещает в SQE операцию RECV, чтение производится в буфер соответствующий клиентскому сокету.
        io_uring_prep_recv(sqe, fd, &bufs[fd], size, 0);

        // Устанавливаем состояние клиентского сокета в READ.
        ConnInfo *conn_i = &conns[fd];
        conn_i->fd = fd;
        conn_i->type = Type::Read;

        // Устанавливаем в поле user_data указатель на socketInfo соответствующий клиентскому сокету.
        io_uring_sqe_set_data(sqe, conn_i);
    }

    void add_socket_write(io_uring *ring,
                          int fd,
                          size_t size)
    {
        // Получаем указатель на первый доступный SQE.
        io_uring_sqe *sqe = io_uring_get_sqe(ring);

        // Хелпер io_uring_prep_send помещает в SQE операцию SEND, запись производится из буфера соответствующего клиентскому сокету.
        io_uring_prep_send(sqe, fd, &bufs[fd], size, 0);

        // Устанавливаем состояние клиентского сокета в WRITE.
        ConnInfo *conn_i = &conns[fd];
        conn_i->fd = fd;
        conn_i->type = Type::Write;

        // Устанавливаем в поле user_data указатель на socketInfo соответсвующий клиентскому сокету.
        io_uring_sqe_set_data(sqe, conn_i);
    }

    void startServer(uint16_t port)
    {
        /** Создаем серверный сокет и начинаем прослушивать порт.
         *  Обратите внимание что при создании сокета мы НЕ УСТАНАВЛИВАЕМ флаг O_NON_BLOCK,
         *  но при этом все чтения и записи не будут блокировать приложение.
         *  Происходит это потому, что io_uring спокойно превращает операции над блокирующими сокетами в non-block системные вызовы.*/
        sockaddr_in serverAddr { AF_INET, htons(port), {.s_addr = INADDR_ANY}, {}};

        int sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        const int val = 1;

        // FIXME: Check result
        ::setsockopt(sock_listen_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

        // FIXME: Check result
        ::bind(sock_listen_fd, (sockaddr*) &serverAddr, sizeof(serverAddr));

        // FIXME: Check result
        ::listen(sock_listen_fd, BACKLOG);

        /** Создаем инстанс io_uring, не используем никаких кастомных опций.
            Емкость очередей SQ и CQ указываем как 4096 вхождений.          **/
        io_uring_params params {};
        io_uring ring {};
        memset(&params, 0, sizeof(params));

        // FIXME: Check result
        ::io_uring_queue_init_params(4096, &ring, &params);

        /** Проверяем наличие фичи IORING_FEAT_FAST_POLL. Для нас это наиболее "перформящая" фича в данном приложении,
            фактически это встроенный в io_uring движок для поллинга I/O.  **/
        if (!(params.features & IORING_FEAT_FAST_POLL)) {
            printf("IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n");
            exit(0);
        }

        /** Добавляем в SQ первую операцию - слушаем сокет сервера для приема входящих соединений. **/
        sockaddr_in client_addr {};
        socklen_t client_len = sizeof(client_addr);
        add_accept(&ring, sock_listen_fd, (struct sockaddr *) &client_addr, &client_len);

        /** event loop **/
        while (true)
        {
            struct io_uring_cqe *cqe;
            [[maybe_unused]] int ret = 0;

            /** Сабмитим все SQE которые были добавлены на предыдущей итерации **/
            io_uring_submit(&ring);

            /** Ждем когда в CQ буфере появится хотя бы одно CQE **/
            // FIXME: Check result
            ret = ::io_uring_wait_cqe(&ring, &cqe);
            // assert(ret == 0);

            /** Положим все "готовые" CQE в буфер cqes  **/
            struct io_uring_cqe *cqes[BACKLOG];
            int cqe_count = io_uring_peek_batch_cqe(&ring, cqes, sizeof(cqes) / sizeof(cqes[0]));

            for (int i = 0; i < cqe_count; ++i)
            {
                cqe = cqes[i];

                /** В поле user_data мы заранее положили указатель структуру  в которой находится служебная информация по сокету. **/
                ConnInfo *user_data = (ConnInfo*) io_uring_cqe_get_data(cqe);

                /** Используя тип идентифицируем операцию к которой относится CQE (accept/recv/send). **/
                Type type = user_data->type;
                if (type == Type::Accept)
                {
                    int sock_conn_fd = cqe->res;

                    /**
                    * Если появилось новое соединение: добавляем в SQ операцию recv - читаем из клиентского сокета,
                    * продолжаем слушать серверный сокет.
                    */
                    add_socket_read(&ring, sock_conn_fd, MAX_MESSAGE_LEN);
                    add_accept(&ring, sock_listen_fd, (struct sockaddr *) &client_addr, &client_len);
                }
                else if (type == Type::Read)
                {
                    int bytes_read = cqe->res;

                    /**
                     * В случае чтения из клиентского сокета: если прочитали 0 байт - закрываем сокет
                     * если чтение успешно: добавляем в SQ операцию send - пересылаем прочитанные данные обратно, на клиент.
                     */
                    if (bytes_read <= 0) {
                        shutdown(user_data->fd, SHUT_RDWR);
                    } else {
                        add_socket_write(&ring, user_data->fd, bytes_read);
                    }
                }
                else if (type == Type::Write)
                {
                    // Запись в клиентский сокет окончена: добавляем в SQ операцию recv - читаем из клиентского сокета.
                    add_socket_read(&ring, user_data->fd, MAX_MESSAGE_LEN);
                }

                io_uring_cqe_seen(&ring, cqe);
            }
        }
    }
}

void IOUringExperiments::TestAll()
{
    Examples::simple_example();
};
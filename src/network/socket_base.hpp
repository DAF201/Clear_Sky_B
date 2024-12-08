#ifndef SOCKET_BASE_HPP
#define SOCKET_BASE_HPP
#ifdef linux
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>
#include <sys/fcntl.h>

namespace clear_sky
{
    // set file descriptor behaviour to non-blocking
    // epoll need this
    void set_nonblocking(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    struct sock_info
    {
        int valid = -1;
        sockaddr_in sock_addr;
        int sock_fd;
        sock_info()
        {
            bzero(&sock_addr, sizeof(sock_addr));
            bzero(&sock_fd, sizeof(sock_fd));
        }

        sock_info(sockaddr_in sock_addr, int sock_fd)
        {
            this->sock_addr = sock_addr;
            this->sock_fd = sock_fd;
            this->valid++;
        }
    };

    class base_socket
    {
    protected:
        int socket_fd = 0;
        struct sockaddr_in sock_address;
        int port;
        std::string IP_address;
        char buffer[1024] = {0};
        int addr_len = sizeof(sock_address);

        // socket flags
        int socket_ready_flag = -2;
        int socket_init_flag = -1;
        int socket_bind_flag = -1;
        int socket_listen_flag = -1;
        int socket_connect_flag = -1;

        base_socket(int port, std::string IP_address = "127.0.0.1")
        {
            this->port = port;

            // init sockaddr_in
            bzero(&sock_address, sizeof(sock_address));
            sock_address.sin_family = AF_INET;

            if (IP_address != "127.0.0.1")
            {
                this->IP_address = IP_address;
                this->sock_address.sin_addr.s_addr = inet_addr(IP_address.c_str());
            }
            this->sock_address.sin_port = htons(port);

            this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd == 0)
            {
                printf("%s\n", "SOCKET CREATION ERROR");
                return;
            }

            socket_ready_flag++;
            socket_init_flag++;
        }

        virtual int socket_bind() noexcept
        {
            if ((socket_init_flag < 0) || (bind(socket_fd, (sockaddr *)&sock_address, sizeof(sock_address)) < 0))
            {
                printf("%s\n", "SOCKET BINDING ERROR");
                return -1;
            }

            socket_ready_flag++;
            socket_bind_flag++;
            return 0;
        }

        virtual int socket_listen(int max_num) noexcept
        {
            if ((socket_bind_flag < 0) || (listen(socket_fd, max_num) < 0))
            {
                printf("%s\n", "SOCKET LISTENING ERROR");
                return -1;
            }

            socket_listen_flag++;
            return 0;
        }

        virtual sock_info socket_accpet() noexcept
        {
            sockaddr_in client_socket_address;
            socklen_t clnt_addr_len = sizeof(client_socket_address);
            int client_socket_fd;

            if ((socket_listen_flag < 0) || (client_socket_fd = accept(socket_fd, (sockaddr *)&client_socket_address, &clnt_addr_len) < 0))
            {
                printf("%s\n", "SOCKET ACCEPTING ERROR");
                return sock_info();
            }

            return sock_info(client_socket_address, client_socket_fd);
        }

        virtual sock_info socket_connect(std::string IP_address, int port) noexcept
        {
            if (socket_init_flag < 0)
            {
                printf("%s\n", "SOCKET CONNECTING ERROR");
                return sock_info();
            }

            sockaddr_in server_address;
            socklen_t server_address_len = sizeof(server_address);
            bzero(&server_address, sizeof(server_address));
            server_address.sin_family = AF_INET;
            server_address.sin_addr.s_addr = inet_addr(IP_address.c_str());
            server_address.sin_port = htons(port);

            if (connect(socket_fd, (sockaddr *)&server_address, server_address_len) < 0)
            {
                printf("%s\n", "SOCKET CONNECTING ERROR");
                return sock_info();
            }
            socket_ready_flag++;
            socket_connect_flag++;
            return sock_info(server_address, socket_fd);
        }

        virtual int socket_send(char *data, int data_size, int sock)
        {
            if (socket_init_flag < 0)
                return -1;
            return send(sock, data, data_size, 0);
        }

        virtual int socket_recv(int sock)
        {
            if (socket_init_flag < 0)
                return -1;
            return recv(sock, buffer, 1024, 0);
        }

        virtual char *get_buffer()
        {
            return buffer;
        }

        virtual std::string get_ip()
        {
            return this->IP_address;
        }
    };
} // namespace clear_sky

#endif
#endif
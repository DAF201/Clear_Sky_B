#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>
#include <sys/fcntl.h>
#include <set>
#include "../event_system/event_system.hpp"
// #include "src/event_system/event_system.hpp"
namespace network
{
    std::mutex fd_lock;
    class __socket
    {
    public:
        int socket_is_valid = 0;
        int this_socket_fd = 0;
        sockaddr_in this_socket_address;
        socklen_t this_socket_address_len = sizeof(this_socket_address);
        std::string host = "0.0.0.0";
        int port = 0;
        int max_connections = 1024;
        template <typename host_name>
        // when use as server, put '0.0.0.0', otherwise will be target info
        __socket(host_name host, int port, int max_connections = 1024)
        {
            this->max_connections = max_connections;
            this->host = host;
            this->port = port;
            this->this_socket_address.sin_family = AF_INET;
            this->this_socket_address.sin_addr.s_addr = inet_addr(this->host.c_str());
            this->this_socket_address.sin_port = htons(port);
            this->this_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            // set fd to non-block for epoll
            event_system::set_nonblocking(this_socket_fd);
            if (this_socket_fd > 0)
                socket_is_valid++;
        }

        virtual int __bind()
        {
            return bind(this->this_socket_fd, (sockaddr *)&this->this_socket_address, this->this_socket_address_len);
        }

        virtual int __listen()
        {
            return listen(this->this_socket_fd, this->max_connections);
        }

        virtual int __accept()
        {
            return accept(this->this_socket_fd, (sockaddr *)&this->this_socket_address, &this->this_socket_address_len);
        }

        virtual int __connect()
        {
            return connect(this->this_socket_fd, (sockaddr *)&this->this_socket_address, this->this_socket_address_len);
        }

        virtual int __read(char *buffer, int buffer_size)
        {
            return read(this->this_socket_fd, buffer, buffer_size);
        }

        virtual int __write(char *buffer, int buffer_size)
        {
            return write(this->this_socket_fd, buffer, buffer_size);
        }

        virtual int __close()
        {
            if (0 == close(this->this_socket_fd))
                this->socket_is_valid--;
            return this->socket_is_valid < 1 ? 0 : -1;
        }

        virtual int socket_validation()
        {
            return this->socket_is_valid > 0;
        }
    };

#define swap_client(fd)                        \
    std::lock_guard<std::mutex> lock(fd_lock); \
    this->client_socket_fd = fd;

    class __server : __socket
    {
    public:
        int epoll_fd;
        epoll_event ev;
        epoll_event ev_buffer[1024];
        std::thread server_thread;

        // reusable
        int client_socket_fd;
        sockaddr_in client_socket_addr;
        socklen_t client_socket_addr_len;
        std::map<int, sockaddr_in> clietn_fd_addr_map;
        std::set<int> client_fd_set;
        template <typename host_name>
        __server(host_name host, int port) : __socket(host, port)
        {
            this->__bind();
            this->__listen();
        }

        int __accept() override final
        {
            client_socket_fd = accept(this->this_socket_fd, (sockaddr *)&this->client_socket_addr, &this->client_socket_addr_len);
            if (client_socket_fd < 0)
            {
                printf("%s", "FAILED TO ACCEPT CONNECTION\n");
                return -1;
            }

            // new connection
            if (client_fd_set.count(client_socket_fd) == 0)
            {
                client_fd_set.insert(client_socket_fd);
                clietn_fd_addr_map[client_socket_fd] = client_socket_addr;
                return client_socket_fd;
            }

            return -1;
        }

        int __write(char *data, int data_size) override final
        {
            return write(client_socket_fd, data, data_size);
        }

        int send(int client_fd, char *data, int data_size)
        {
            swap_client(client_fd);
            return this->__write(data, data_size);
        }

        int __read(char *buffer, int buffer_size) override final
        {

            return read(client_socket_fd, buffer, buffer_size);
        }

        int recv(int client_fd, char *buffer, int buffer_size)
        {
            swap_client(client_fd);
            return this->__read(buffer, buffer_size);
        }

        void start()
        {
            epoll_fd = epoll_create1(0);
            ev.data.fd = this->this_socket_fd;
            ev.events = EPOLLIN;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->this_socket_fd, &ev);
            server_thread = std::thread(&__server::run, this);
        }

        void run()
        {
            int num_of_waiting_connections = 0;
            while (socket_is_valid)
            {
                num_of_waiting_connections = epoll_wait(epoll_fd, ev_buffer, 1024, 500);
                for (int i = 0; i < num_of_waiting_connections; i++)
                {
                    if (ev_buffer[i].data.fd = this->this_socket_fd)
                    {
                        int client_socket_fd = this->__accept();
                    }
                }
            }
        }
    };

    class __client : __socket
    {
    public:
        template <typename host_name>
        __client(host_name host, int port) : __socket(host, port)
        {
            this->__connect();
        }
    };
}
#endif
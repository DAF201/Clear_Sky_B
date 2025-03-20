#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP
#include "socket_base.hpp"

namespace network
{
    class server_socket : base_socket
    {
    private:
        static std::map<event_system::fd, sockaddr_in> client_sockfd_address_map;
        static event_system::listener event_listener;
        static void accept_connection(void *socket_fd, void *reutrn_ptr)
        {
            struct sockaddr_in client_address
            {
            };
            socklen_t client_len = sizeof(client_address);
            int client_fd = accept(*((int *)socket_fd), (sockaddr *)&client_address, &client_len);
            if (client_fd < 0)
            {
                printf("accept failed");
                return;
            }
            set_nonblocking(client_fd);
            // save address
            client_sockfd_address_map[client_fd] = client_address;
        }
        
        void run()
        {
        }

    public:
        server_socket(int port, std::string IP_address, int max_num) : base_socket::base_socket(port, IP_address)
        {
            // start listen
            socket_listen(max_num);
            event_listener.start();
            // set task, accept connection, take socket fd, create connection, no return, not urgent, important, always repeat when possible
            // add task to listener, trigger when new connection comming in
            event_listener.add_event(socket_fd, EPOLLIN | EPOLLET, {this->accept_connection, &socket_fd, nullptr, event_system::NOT, event_system::VERY, INT32_MAX});
        }

        void start()
        {
            // TODO: start to accept the connections, and handle them
            std::thread t = std::thread(&server_socket::run, this);
        }
    };
} // namespace network
#endif
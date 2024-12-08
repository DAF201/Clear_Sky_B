#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP
#include "socket_base.hpp"
namespace clear_sky
{
    class server_socket : base_socket
    {
    private:

        std::map<int, sockaddr_in> client_sockdf_address_map;

    public:
        server_socket(int port, std::string IP_address, int max_num) : base_socket::base_socket(port, IP_address)
        {
            socket_listen(max_num);
        }
    };
} // namespace clear_sky
#endif
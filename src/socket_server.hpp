#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP
#include "socket_base.hpp"
namespace clear_sky
{
    class server_socket : base_socket
    {
    private:
    public:
        server_socket(int port, std::string IP_address) : base_socket::base_socket(port, IP_address)
        {
            if (base_socket::socket_bind() < 0)
            {
                printf("%s", "binding error");
                return;
            }
        }
    };
} // namespace clear_sky
#endif
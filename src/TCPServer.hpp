#include "Server.hpp"
#include "Connection.hpp"

class TCPServer : Server {
  public:
    TCPServer();
    ~TCPServer();
  private:
    void m_handle_accept(/* here something */);
};

///////////////////////////////////////////////////////////////////////////////
//
//  TCPSerber.hpp specifies a class which define a server using the TCP 
//  protocol.
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/asio.hpp>
#include "Server.hpp"

class TCPServer : public Server {
  using boost::asio::ip::tcp;

  public:
    TCPServer(int);
    ~TCPServer();
    void respond(void);
  private:
    //Variables
    tcp::acceptor m_acceptor;
    //Functions
    void m_start_accept(void);
    void m_handle_accept(m_connection_database_record_t &,
                         const boost::system::error_code &);
};

TCPServer::TCPServer(int port) : m_acceptor(m_io_context, 
                                            tcp::endpoint(tcp::v4(), port))
{
  m_start_accept();
}

TCPServer::~TCPServer() {}

void m_handle_accept(m_connection_database_record_t& new_connection,
                     const boost::system::error_code& error)
{
  if(!error) {
    new_connection.remote_ip = ;
    new_connection. = ;
    m_connection_database.insert(new_connection);
  }
  m_start_accept();
}

void Server::start_accept(void) {
  m_connection_database_record_t new_connection;

  async_accept(new_connection.connection_ptr->get_socket(),
               std::bind(&m_handle_accept, this, new_connection, error));
}

void Server::respond(void) {}

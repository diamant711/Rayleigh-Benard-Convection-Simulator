#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Server.hpp"
#include "Connection.hpp"

//Not good here
using boost::asio::ip::tcp;

//! TCPServer.hpp defines a server using the TCP protocol.
/*!
 This class is a derived class from base class Server in public mode.
*/
class TCPServer : public Server {

  public:
    TCPServer(std::shared_ptr<boost::asio::io_context>, int);
    ~TCPServer();
  private:
    //Variables
    tcp::acceptor m_acceptor;
    int m_port;
    //Functions
    void m_start_accept(void);
    void m_handle_accept(m_connection_database_record_t,
                         const boost::system::error_code &);
};

TCPServer::TCPServer(std::shared_ptr<boost::asio::io_context> executor_ptr,
                     int port)
  : Server(executor_ptr),
    m_acceptor(m_get_executor(), tcp::endpoint(tcp::v4(), port))
{
  m_port = port;
  std::cerr << "INFO: TCPServer (" << m_port << "): m_handle_accept: started "
            << "async accept chain operation..." << std::endl;
  m_start_accept();
}

TCPServer::~TCPServer() {}

void TCPServer::m_handle_accept(m_connection_database_record_t new_connection,
                                const boost::system::error_code& error)
{
  std::cerr << "INFO: TCPServer (" << m_port << "): m_handle_accept: async "
            << "accept event happened..." << std::endl;
  if(!error) {
    new_connection.remote_ip =
      new_connection.connection_ptr->get_socket().remote_endpoint().address();
    new_connection.port =
      new_connection.connection_ptr->get_socket().remote_endpoint().port();
    m_insert_new_connection(new_connection);
    std::cerr << "INFO: TCPServer (" << m_port << "): m_handle_accept: new "
              << "connection plugged." << std::endl;
  }
  m_start_accept();
}

void TCPServer::m_start_accept(void) {
  Server::m_connection_database_record_t new_connection;
  new_connection.connection_ptr.reset(new Connection(m_get_executor()));

  m_acceptor.async_accept(new_connection.connection_ptr->get_socket(),
    boost::bind(&TCPServer::m_handle_accept, this, new_connection,
    boost::asio::placeholders::error));
}

#endif

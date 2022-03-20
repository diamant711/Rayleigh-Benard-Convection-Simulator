///////////////////////////////////////////////////////////////////////////////
//
//  TCPSerber.hpp specifies a class which define a server using the TCP 
//  protocol.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "Server.hpp"
#include "Connection.hpp"

//Not good here
using boost::asio::ip::tcp;

class TCPServer : public Server {

  public:
    TCPServer(int);
    ~TCPServer();
    void respond(void);
  private:
    //Variables
    tcp::acceptor m_acceptor;
    //Functions
    void m_start_accept(void);
    void m_handle_accept(m_connection_database_record_t,
                         const boost::system::error_code &);
    
};

TCPServer::TCPServer(int port) : m_acceptor(m_io_context, 
                                            tcp::endpoint(tcp::v4(), port))
{
  m_start_accept();
}

TCPServer::~TCPServer() {}

void TCPServer::m_handle_accept(m_connection_database_record_t new_connection,
                                const boost::system::error_code& error)
{
  if(!error) {
    new_connection.remote_ip = 
      new_connection.connection_ptr->get_socket().remote_endpoint().address();
    new_connection.port = 
      new_connection.connection_ptr->get_socket().remote_endpoint().port();
    m_connection_database.push_back(new_connection);
  }
  m_start_accept();
}

void TCPServer::m_start_accept(void) {
  Server::m_connection_database_record_t new_connection;
  new_connection.connection_ptr.reset(new Connection(m_io_context));

  m_acceptor.async_accept(new_connection.connection_ptr->get_socket(),
    boost::bind(&TCPServer::m_handle_accept, this, new_connection, 
    boost::asio::placeholders::error));
}

void TCPServer::respond(void) {}

#endif

#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Server.hpp"
#include "Connection.hpp"

//! TCPServer.hpp defines a server using the TCP protocol.
/*!
 This class is a derived class from base class Server in public mode.
*/
class TCPServer : public Server {

  public:
    TCPServer(std::shared_ptr<boost::asio::io_context>, int);
    ~TCPServer(void);
  private:
    //Variables
    //! The TCP acceptor type.
    /*!
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/ip__tcp/acceptor.html">ip::tcp::acceptor</a>
    */
    boost::asio::ip::tcp::acceptor m_acceptor;
    //! Specifies the port number.
    int m_port;
    //Functions
    void m_start_accept(void);
    void m_handle_accept(m_connection_database_record_t,
                         const boost::system::error_code &);
};

//! Class constructor.
/*!
 \param executor_ptr shared pointer to <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/io_context.html">boost::asio::io_context</a>
 \param port specifies the port number.
*/
TCPServer::TCPServer(std::shared_ptr<boost::asio::io_context> executor_ptr,
                     int port)
  : Server(executor_ptr),
    m_acceptor(m_get_executor(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
  m_port = port;
  std::cerr << "INFO: TCPServer (" << m_port << "): m_handle_accept: started "
            << "async accept chain operation..." << std::endl;
  m_start_accept();
}

//! Class destructor.
TCPServer::~TCPServer(void) {}

//! This function handles the accept operation.
/*!
 It reports if the async_accept event happened and if the new connection results
 plugged without errors.
 \param new_connection connection database record defined in Connection.hpp
 \param error <a href="https://www.boost.org/doc/libs/1_79_0/libs/system/doc/html/system.html">boost::system::error_code</a>
 \sa <a href="https://www.boost.org/doc/libs/1_79_0/boost/system/error_code.hpp">boost::system::error_code.hpp</a>
*/
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

/*!
 This function is implemented with
 <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_socket_acceptor/async_accept.html">async_accept</a> 
 ip::tcp::acceptor member function.
*/
void TCPServer::m_start_accept(void) {
  Server::m_connection_database_record_t new_connection;
  new_connection.connection_ptr.reset(new Connection(m_get_executor()));

  m_acceptor.async_accept(new_connection.connection_ptr->get_socket(),
    boost::bind(&TCPServer::m_handle_accept, this, new_connection,
    boost::asio::placeholders::error));
}

#endif

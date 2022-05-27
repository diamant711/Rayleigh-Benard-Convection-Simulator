//  \sa <a href="">My external page</a>


#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>

#include <boost/asio.hpp>

#include "Connection.hpp"

//! Server.hpp defines the base virtual methods for an asynchronus server.
/*!
 The server is built  asynchronous, so execution of the server
 application is not suspended while it waits for a connection from a client.
 This class works with Boost.Asio.
*/
class Server {
  public:
    Server(boost::asio::io_context &);
    ~Server(void);
    void poll(void);
  protected:
    //Variables

    //! This struct specifies the database's record.
    /*!
     \param status_id record field.
     \param end_of_life record field, default setting at false.
     \param remote_ip record field: <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/ip__address.html">boost::asio::ip::address</a>
     \param port record field.
    */
    typedef struct {
      std::shared_ptr<Connection> connection_ptr;
      /* TODO: check if implemented.
       *   status_id  = 0 UNCATEGORIZED
       *   status_id != 0 inheritance categorization
       */
      int status_id = 0;
      bool end_of_life = false;
      boost::asio::ip::address remote_ip;
      int port = -1;
    } m_connection_database_record_t;
    //Functions
    boost::asio::io_context& m_get_executor(void);
    m_connection_database_record_t& m_get_connection_by_index(int);
    m_connection_database_record_t& m_get_first_connection(void);
    m_connection_database_record_t& m_get_last_connection(void);
    size_t m_get_plugged_connection(void);
    bool m_is_waiting_list_empty(void);
    void m_insert_new_connection(m_connection_database_record_t &);
    void m_delete_connection_by_index(int);
    //! This virtual method regulates when the server starts to accept connections.  
    virtual void m_start_accept(void) {};
  private:
    //! Unique pointer at boost::asio::io_context.
    std::unique_ptr<boost::asio::io_context> m_io_context_ptr;
    //! std::vector of m_connection_database_record_t that represents the class database.
    std::vector<m_connection_database_record_t> m_connection_database;
};

//! Class constructor.
/*!
 \param executor boost::asio::io_context provides core I/O functionality.
 \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/io_context.html">boost::asio__io_context</a>
*/
Server::Server(boost::asio::io_context& executor) {
  m_io_context_ptr.reset(&executor);
}

//! Class destructor.
Server::~Server(void) {}

//! This method returns the executor associated with the io_context.
boost::asio::io_context& Server::m_get_executor(void) {
  return *m_io_context_ptr;
}

//! Returns a specific connection via its index.
Server::m_connection_database_record_t& Server::m_get_connection_by_index(int index) {
  return m_connection_database.at(index);
}

//! Returns the first created connection.
Server::m_connection_database_record_t& Server::m_get_first_connection(void) {
  return m_connection_database.front();
}

//! Returns the last created connection.
Server::m_connection_database_record_t& Server::m_get_last_connection(void) {
  return m_connection_database.back();
}

//! Retrurns the number of existing connections.
size_t Server::m_get_plugged_connection(void) {
  return m_connection_database.size();
}

//! Inserts a new connection via push_back().
void Server::m_insert_new_connection(Server::m_connection_database_record_t& new_connection) {
  m_connection_database.push_back(new_connection);
}

//! Deletes a specific connection based on its index.
void Server::m_delete_connection_by_index(int index) {
  m_connection_database.erase(m_connection_database.cbegin() + index);
}

//! Returns true if waiting list is empty.
bool Server::m_is_waiting_list_empty(void) {
  return m_connection_database.empty();
}

//! Calls the poll function from Boost.Asio.
/*!
 \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/io_context/poll.html">boost::asio::io_context::poll()</a> 
*/
void Server::poll(void) {
  m_io_context_ptr->poll();
}

#endif

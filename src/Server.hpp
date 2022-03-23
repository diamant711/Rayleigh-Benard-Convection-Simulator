///////////////////////////////////////////////////////////////////////////////
//
//  Serber.hpp specifies a class which define the base virtual methods that 
//  define an asyncronus server
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <boost/asio.hpp>
#include "Connection.hpp"

class Server {
  public:
    Server();
    ~Server();
  protected:
    //Variables
    typedef struct {
      std::shared_ptr<Connection> connection_ptr;
      /*
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
    m_connection_database_record_t& m_get_fist_connection(void);
    m_connection_database_record_t& m_get_last_connection(void);
    size_t m_get_plugged_connection(void);
    void m_insert_new_connection(m_connection_database_record_t &);
    virtual void m_start_accept(void) {};
  private:
    boost::asio::io_context m_io_context;
    std::vector<m_connection_database_record_t> m_connection_database;
};

Server::Server() {}

Server::~Server() {}

boost::asio::io_context& Server::m_get_executor(void) {
  return m_io_context;
}

Server::m_connection_database_record_t& Server::m_get_connection_by_index(int index) {
  return m_connection_database.at(index);
}

Server::m_connection_database_record_t& Server::m_get_fist_connection(void) {
  return m_get_connection_by_index(0);
}

Server::m_connection_database_record_t& Server::m_get_last_connection(void) {
  return m_get_connection_by_index(m_get_plugged_connection() - 1);
}

size_t Server::m_get_plugged_connection(void) {
  return m_connection_database.size();
}

void Server::m_insert_new_connection(Server::m_connection_database_record_t& new_connection) {
  m_connection_database.push_back(new_connection);
}

#endif

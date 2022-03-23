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
    boost::asio::io_context& get_executor(void);
    m_connection_database_record_t& get_connection_by_index(int);
    m_connection_database_record_t& get_fist_connection(void);
    m_connection_database_record_t& get_last_connection(void);
    size_t get_plugged_connection(void);
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
    virtual void m_start_accept(void) {};
  private:
    boost::asio::io_context m_io_context;
    std::vector<m_connection_database_record_t> m_connection_database;
};

Server::Server() {}

Server::~Server() {}

boost::asio::io_context& Server::get_executor(void) {
  return m_io_context;
}

Server::m_connection_database_record_t& get_connection_by_index(int index) {
  return m_connection_database.at(index);
}

Server::m_connection_database_record_t& get_fist_connection(void) {
  return get_connection_by_index(0);
}

Server::m_connection_database_record_t& get_last_connection(void) {
  return get_connection_by_index(get_plugged_connection() - 1);
}

size_t Server::get_plugged_connection(void) {
  return m_connection_database.size();
}

#endif

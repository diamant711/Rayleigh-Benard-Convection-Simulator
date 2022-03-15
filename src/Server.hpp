///////////////////////////////////////////////////////////////////////////////
//
//  Serber.hpp specifies a class which define the base virtual methods that 
//  define an asyncronus server
//
///////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <boost/asio.hpp>
#include "Connection.hpp"

class Server {
  public:
    Server();
    ~Server();
    virtual void respond(void);
    boost::asio::io_context& get_executor(void);
  protected:
    //Variables
    typedef struct {
      std::unique_ptr<Connection> connection_ptr;
      /*
       *   status_id  = 0 UNCATEGORIZED
       *   status_id != 0 inheritance categorization
       */
      int status_id = 0;
      bool end_of_life = false;
      boost::asio::ip::address remote_ip;
      int port;
    } m_connection_database_record_t;
    std::vector<m_connection_database_record_t> m_connection_database;
    boost::asio::io_context m_io_context;
    //Functions
    virtual void m_start_accept(void);
};

Server::Server() {}

Server::~Server() {}

boost::asio::io_context& Server::get_executor(void) {
  return m_io_context;
}

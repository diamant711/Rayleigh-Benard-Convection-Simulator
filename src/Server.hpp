///////////////////////////////////////////////////////////////////////////////
//
//  Serber.hpp specifies a class which define the base virtual methods that 
//  define an asyncronus server
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/asio.hpp>
#include "Connection.hpp"

class Server {
  public:
    Server();
    ~Server();
    virtual void accept(void);
    virtual void close(void); /*Garbage collector bad*/
  protected:
    //Variables
    typedef struct {
      shared_ptr<Connection> connection_ptr;
      int status_id;
      bool end_of_life;
      boost::asio::address remote_ip;
    } m_connection_database_record_t;
    std::map<int, m_connection_database_record_t> m_connection_database;
    boost::asio::io_context m_io_context;
};

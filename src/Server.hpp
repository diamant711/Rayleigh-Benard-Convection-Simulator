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
    Server(boost::asio::io_context &);
    ~Server();
    virtual void accept(void);
    virtual void respond(void);
  protected:
    //Variables
    typedef struct {
      Connection connection;
      int status_id;
      bool end_of_life = false;
      boost::asio::address remote_ip;
    } m_connection_database_record_t;
    std::map<int, m_connection_database_record_t> m_connection_database;
    boost::asio::io_context m_io_context;
};

Server::Server(boost::asio::io_context& tmp) : m_io_context(tmp) {}

Server::~Server() {}

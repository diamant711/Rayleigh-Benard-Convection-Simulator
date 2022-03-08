///////////////////////////////////////////////
//
//  Serber.hpp specifies a class which define
//  the base virtual methods that define a server
//
/////////////////////////////////////////////////
#include <boost/asio.hpp>

class Server {
  public:
    virtual void accept(void);
    virtual void reply(const std::function &);
  private:
    boost::asio::io_context m_io_context;
};

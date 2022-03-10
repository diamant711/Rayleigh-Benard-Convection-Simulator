///////////////////////////////////////////////
//
//  Serber.hpp specifies a class which define
//  the base virtual methods that define a server
//
/////////////////////////////////////////////////
#include <boost/asio.hpp>

class Server {
  public:
    Server();
    ~Server();
    virtual void accept(void);
    virtual void reply(const std::function<std::string& (void)> &);
    const std::string& get_remote_ip() const;
  private:
    boost::asio::io_context m_io_context;
    const boost::asio::ip::address m_remote_address;
    bool m_connected = false;
};

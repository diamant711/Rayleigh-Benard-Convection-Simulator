///////////////////////////////////////////////////////////////////////////////
//
//    Connection.hpp specifies a class that represent a connection and define
//    the wrapper function for input-output socket operation using an internal
//    buffer
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/asio.hpp>
#include <memory>

class Connection {
  public:
    Connection();
    ~Connection();
    bool send(void);
    bool recive(void);
    void load_data(const std::string &);
    void load_data(const std::vector<int> &);
    void load_data(const std::vector<unsigned char> &);
    void load_data(const std::shared_ptr<char>);
    void load_data(const boost::asio::mutable_buffer &);
    const std::shared_ptr<char> unload_data(void) const;
    boost::asio::ip::tcp::socket& get_socket();
  private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::mutable_buffer m_internal_buffer;
};

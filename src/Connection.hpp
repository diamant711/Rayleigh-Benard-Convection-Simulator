///////////////////////////////////////////////////////////////////////////////
//
//    Connection.hpp specifies a class that represent a connection and define
//    the wrapper function for input-output socket operation using an internal
//    buffer
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <memory>

#include <boost/asio.hpp>

class Connection {
  public:
    Connection(boost::asio::io_context &);
    ~Connection(void);
    bool send(void);
    bool recive(void);
    void load_data(const std::string &);
    void load_data(const std::vector<int> &);
    void load_data(const std::vector<unsigned char> &);
    void load_data(const std::shared_ptr<char>);
    void load_data(const boost::asio::mutable_buffer &);
    const std::shared_ptr<char> unload_data(void) const;
    boost::asio::ip::tcp::socket& get_socket(void);
  private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::mutable_buffer m_internal_buffer;
};

Connection::Connection(boost::asio::io_context& executor) : m_socket(executor) {}
Connection::~Connection(void) {}
bool Connection::send(void) {
  return false;
}
bool Connection::recive(void) {
  return false;
}
void Connection::load_data(const std::string &) {}
void Connection::load_data(const std::vector<int> &) {}
void Connection::load_data(const std::vector<unsigned char> &) {}
void Connection::load_data(const std::shared_ptr<char>) {}
void Connection::load_data(const boost::asio::mutable_buffer &) {}
const std::shared_ptr<char> Connection::unload_data(void) const {
  std::shared_ptr<char> data_ptr(static_cast<char *>(m_internal_buffer.data()));
  return data_ptr;
}
boost::asio::ip::tcp::socket& Connection::get_socket(void) {
  return m_socket;
}

#endif

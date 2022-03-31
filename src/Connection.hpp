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
#include <boost/bind/bind.hpp>


class Connection {
  public:
    Connection(boost::asio::io_context &);
    ~Connection(void);
    void send(void);
    void receive(void);
    void load_data(const std::string &);
    void load_data(const std::vector<int> &);
    void load_data(const std::vector<unsigned char> &);
    void load_data(const std::shared_ptr<char>);
    void load_data(const boost::asio::const_buffer &);
    const std::shared_ptr<char> unload_data(void) const;
    boost::asio::ip::tcp::socket& get_socket(void);
  private:
    //variables
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::mutable_buffer m_internal_receive_buffer;
    boost::asio::mutable_buffer m_internal_send_buffer; //cast a const_buffer
    bool m_send_error = false;
    bool m_receive_error = false;
    //functions
    void m_handle_send(const boost::system::error_code&,
                       size_t /*bytes_transferred*/);
};

Connection::Connection(boost::asio::io_context& executor) : m_socket(executor) {}

Connection::~Connection(void) {
m_socket.close();
}

void Connection::m_handle_send(const boost::system::error_code& error, size_t bytes_transferred){
  if (!error){
    ::memset(m_internal_send_buffer.data(), 0, m_internal_send_buffer.size());
  } else {
    m_send_error = true; //va implementato come controllo nei send successivi notifica utente
  }
}

void Connection::send(void) {
  m_socket.async_send(m_internal_send_buffer, boost::bind(&Connection::m_handle_send, this, boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
}

void Connection::receive(void) {
//  m_socket.async_receive(m_internal_receive_buffer);
}

void Connection::load_data(const std::string &) {}

void Connection::load_data(const std::vector<int> &) {}

void Connection::load_data(const std::vector<unsigned char> &) {}

void Connection::load_data(const std::shared_ptr<char>) {}

void Connection::load_data(const boost::asio::const_buffer &) {}

const std::shared_ptr<char> Connection::unload_data(void) const {
  std::shared_ptr<char> data_ptr(static_cast<char *>(m_internal_receive_buffer.data()));
  return data_ptr;
}
boost::asio::ip::tcp::socket& Connection::get_socket(void) {
  return m_socket;
}

#endif

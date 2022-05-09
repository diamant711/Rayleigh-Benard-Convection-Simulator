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
    void load_data(const boost::asio::const_buffer &);
    const std::shared_ptr<char> unload_data(void) const;
    boost::asio::ip::tcp::socket& get_socket(void);
    bool first_operation_ended(void);
  private:
    // Variables
    boost::asio::ip::tcp::socket m_socket;
    std::unique_ptr<boost::asio::mutable_buffer> m_internal_receive_buffer_ptr;
    std::unique_ptr<boost::asio::const_buffer> m_internal_send_buffer_ptr;
    bool m_send_error = false;
    bool m_receive_error = false;
    bool m_first_operation_ended = false;
    // Functions
    void m_handle_send(const boost::system::error_code&,
                       size_t /*bytes_transferred*/);
    void m_handle_receive(const boost::system::error_code&, size_t);
    void m_resize_internal_send_buffer(int n);
};

Connection::Connection(boost::asio::io_context& executor) : m_socket(executor) {}

Connection::~Connection(void) {
  m_socket.close();
}

void Connection::m_handle_send(const boost::system::error_code& error, size_t bytes_transferred){
  if (!error){
    m_internal_receive_buffer_ptr.release();
    m_first_operation_ended = true;
  } else {
    m_send_error = true; //va implementato come controllo nei send successivi notifica utente
    std::cerr << "Error: send: byte transferred = " << bytes_transferred << std::endl;
  }
}

void Connection::m_handle_receive(const boost::system::error_code& error, size_t bytes_transferred){
 if (!error){
   m_internal_send_buffer_ptr.release();
   m_first_operation_ended = true;
  } else {
    m_receive_error = true; //va implementato come controllo nei receive successivi notifica utente
    std::cerr << "Error: receive: byte transferred = " << bytes_transferred << std::endl;
  }
}


void Connection::send(void) {
  m_socket.async_send(*m_internal_send_buffer_ptr, 
                      boost::bind(&Connection::m_handle_send, this, 
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void Connection::receive(void) {
  m_socket.async_receive(*m_internal_receive_buffer_ptr,
                        boost::bind(&Connection::m_handle_receive, this, 
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
}

void Connection::load_data(const std::string &input) {
  m_internal_send_buffer_ptr.reset(new boost::asio::const_buffer(input.data(), 
                                                                 input.size()));
}

void Connection::load_data(const std::vector<int> &input) {
}

void Connection::load_data(const std::vector<unsigned char> &input) {
}

void Connection::load_data(const boost::asio::const_buffer &input) {
}

const std::shared_ptr<char> Connection::unload_data(void) const {
}

boost::asio::ip::tcp::socket& Connection::get_socket(void) {
  return m_socket;
}
 
bool Connection::first_operation_ended(void) {
  return m_first_operation_ended;
}

#endif

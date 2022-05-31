
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <memory>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

//! Connection defines the wrapper functions for input-output socket operation using an internal buffer.
/*!
*/
class Connection {
  public:
    Connection(boost::asio::io_context &);
    ~Connection(void);
    void send(void);
    void receive(void);
    void load_data(const std::string &);
    void load_data(const std::vector<unsigned char> &);
    void load_data(const boost::asio::const_buffer &);
    //!
    const std::shared_ptr<char[]> unload_data(void) const;
    //!
    boost::asio::ip::tcp::socket& get_socket(void);
    //!
    bool first_operation_ended(void);
    //!
    bool is_ready_to_receive(void);
    //!
    bool is_persistant(void);
    //!
    void set_persistant(void);
    //!
    bool is_ready_to_send(void);
  private:
    // Variables
    /*!
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/ip__tcp/socket.html">boost::asio::ip::tcp::socket</a>
   */
    boost::asio::ip::tcp::socket m_socket;

    /*!
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/mutable_buffer.html">boost::asio::mutable_buffer</a>
    */
    std::unique_ptr<boost::asio::mutable_buffer> m_internal_receive_buffer_ptr;
    /*!
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/const_buffer.html">boost::asio::const_buffer</a>
   */
    std::unique_ptr<boost::asio::const_buffer> m_internal_send_buffer_ptr;

    bool m_send_error = false;

    bool m_receive_error = false;

    bool m_first_operation_ended = false;

    bool m_ready_to_send = true;

    bool m_persistant = false;
    // Functions
    void m_handle_send(const boost::system::error_code&,
                       size_t /*bytes_transferred*/);
    void m_handle_receive(const boost::system::error_code&, size_t);
};

//! Class constructor.
Connection::Connection(boost::asio::io_context& executor) : m_socket(executor) {}

//! Class destructor.
Connection::~Connection(void) {
  //std::cerr << "INFO: Connection: user " << m_socket.remote_endpoint().address() 
  //          << ": destructor: socket close." << std::endl;
  //m_socket.close();
}

//!
/*!
\param error
\param bytes_transferred
*/
void Connection::m_handle_send(const boost::system::error_code& error, size_t bytes_transferred){
  std::cerr << "INFO: Connection: user " << m_socket.remote_endpoint().address() 
            << ": m_handle_send: bytes transferred = " << bytes_transferred 
            << std::endl;
  if (!error){
    m_internal_send_buffer_ptr.release();
    m_first_operation_ended = true;
    m_ready_to_send = true;
  } else {
    m_send_error = true;
    std::cerr << "Error" << std::endl;
  }
}

//!
/*!
\param error
\param bytes_transferred
*/
void Connection::m_handle_receive(const boost::system::error_code& error, size_t bytes_transferred){
  std::cerr << "INFO: Connection: user " << m_socket.remote_endpoint().address() 
            << ": m_handle_receive: bytes transferred = " << bytes_transferred 
            << std::endl;
  if (!error){
   m_internal_receive_buffer_ptr.release();
   m_first_operation_ended = true;
  } else {
    m_receive_error = true;
    std::cerr << "Error" << std::endl;
  }
}

//!
void Connection::send(void) {
  boost::asio::async_write(
      m_socket, *m_internal_send_buffer_ptr, 
      boost::bind(&Connection::m_handle_send, this, 
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  m_ready_to_send = false;
}

//!
void Connection::receive(void) {
  m_internal_receive_buffer_ptr.reset(new boost::asio::mutable_buffer(
    new char[m_socket.available()], m_socket.available()
  ));
  m_socket.async_receive(*m_internal_receive_buffer_ptr,
                        boost::bind(&Connection::m_handle_receive, this, 
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
}
//!
/*!
\param input
*/
void Connection::load_data(const std::string &input) {
  m_internal_send_buffer_ptr.reset(new boost::asio::const_buffer(input.data(),
                                                                 input.size()));
}

//!
/*!
\param input
*/
void Connection::load_data(const std::vector<unsigned char> &input) {
  m_internal_send_buffer_ptr.reset(new boost::asio::const_buffer(input.data(), 
                                                                 input.size()));
}

//!
/*!
\param input
*/
void Connection::load_data(const boost::asio::const_buffer &input) {
  m_internal_send_buffer_ptr.reset(new boost::asio::const_buffer(input.data(), 
                                                                 input.size()));
}

//!
const std::shared_ptr<char[]> Connection::unload_data(void) const {
  size_t size = m_internal_receive_buffer_ptr->size();
  std::shared_ptr<char[]> ret_ptr;
  ret_ptr.reset(new char[size]);
  ::memcpy(ret_ptr.get(), m_internal_receive_buffer_ptr->data(), 
                          m_internal_receive_buffer_ptr->size());
  return ret_ptr;
}

//!
boost::asio::ip::tcp::socket& Connection::get_socket(void) {
  return m_socket;
}

//!
bool Connection::first_operation_ended(void) {
  return m_first_operation_ended;
}

//!
bool Connection::is_ready_to_receive(void) {
  if(m_socket.available() > 0) {
    return true;
  } else {
    return false;
  }
}

//!
void Connection::set_persistant(void) {
  m_persistant = true;
}

//!
bool Connection::is_persistant(void) {
  return m_persistant;
}

//!
bool Connection::is_ready_to_send(void) {
  return m_ready_to_send;
}

#endif

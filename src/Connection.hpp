
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
    const std::shared_ptr<char[]> unload_data(void) const;
    boost::asio::ip::tcp::socket& get_socket(void);
    bool first_operation_ended(void);
    bool is_ready_to_receive(void);
    bool is_persistant(void);
    void set_persistant(void);
    bool is_ready_to_send(void);
  private:
    // Variables
    /*!
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/ip__tcp/socket.html">boost::asio::ip::tcp::socket</a>
   */
    boost::asio::ip::tcp::socket m_socket;
    //! Holds a buffer that can be modified.
    /*!
     Used during the input operations.
     \sa m_handle_receive
     \sa receive
     \sa unload_data
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/mutable_buffer.html">boost::asio::mutable_buffer</a>
    */
    std::unique_ptr<boost::asio::mutable_buffer> m_internal_receive_buffer_ptr;
    //! Holds a buffer that cannot be modified.
    /*!
     Used during the output operations.
     \sa m_handle_send
     \sa send
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/const_buffer.html">boost::asio::const_buffer</a>
   */
    std::unique_ptr<boost::asio::const_buffer> m_internal_send_buffer_ptr;
    //! Checks if errors occurred during the send operation.
    bool m_send_error = false;
    //! Checks if errors occurred during the receive operation.
    bool m_receive_error = false;
    //! Checks if the first operation has endend.
    /*!
     \sa first_operation_ended
    */
    bool m_first_operation_ended = false;
    //!
    bool m_ready_to_send = true;
    //! 
    bool m_persistant = false;
    // Functions
    void m_handle_send(const boost::system::error_code&,
                       size_t /*bytes_transferred*/);
    void m_handle_receive(const boost::system::error_code&, size_t);
};

//! Class constructor.
/*!
\param executor <a href="https://www.boost.org/doc/libs/master/doc/html/boost_asio/reference/io_context.html">boost::asio::io_context& executor</a>
*/
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

//! Completion handler, it is called when the receive completes.
/*!
\param error
\param bytes_transferred
\sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_stream_socket/async_receive/overload2.html">async_receive</a>
*/
void Connection::m_handle_receive(const boost::system::error_code& error, size_t bytes_transferred){
  std::cerr << "INFO: Connection: user " << m_socket.remote_endpoint().address()
            << ": m_handle_receive: bytes transferred = " << bytes_transferred
            << std::endl;
  if (!error){
   m_first_operation_ended = true;
  } else {
    m_receive_error = true;
    std::cerr << "Error" << std::endl;
  }
}

//!
/*!
\sa <a href="">async_write</a>
*/
void Connection::send(void) {
  boost::asio::async_write(
      m_socket, *m_internal_send_buffer_ptr,
      boost::bind(&Connection::m_handle_send, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  m_ready_to_send = false;
}

//! 
/*!
\sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_stream_socket/async_receive/overload2.html">async_receive</a>
*/
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
  m_internal_receive_buffer_ptr.release();
  m_internal_send_buffer_ptr.reset(new boost::asio::const_buffer(input.data(),
                                                                 input.size()));
}

//!
/*!
\param input
*/
void Connection::load_data(const std::vector<unsigned char> &input) {
  m_internal_receive_buffer_ptr.release();
  m_internal_send_buffer_ptr.reset(new boost::asio::const_buffer(input.data(), 
                                                                 input.size()));
}

//!
/*!
\param input
*/
void Connection::load_data(const boost::asio::const_buffer &input) {
  m_internal_receive_buffer_ptr.release();
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

//! This function returns true if the first operation has endend.
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

//! Sets a connection as persistant.
/*!
\sa m_persistant
*/
void Connection::set_persistant(void) {
  m_persistant = true;
}

//! Checks if the connection is persistant.
/*!
\sa m_persistant
*/
bool Connection::is_persistant(void) {
  return m_persistant;
}

//! Checks if the connection is ready to send.
/*!
m_ready_to_send is set false by the function send, and is set true again by the m_handle_send function.
\sa send
*/
bool Connection::is_ready_to_send(void) {
  return m_ready_to_send;
}

#endif

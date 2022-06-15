#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <memory>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

//! Connection defines the wrapper functions for input-output socket operation using an internal buffer.

class Connection
{
public:
  Connection (boost::asio::io_context &);
  ~Connection (void);
  void send (void);
  void receive (void);
  void load_data (const std::string &);
  void load_data (const std::vector<unsigned char> &);
  void load_data (const boost::asio::const_buffer &);
  const std::shared_ptr<char[]> unload_data (void) const;
  boost::asio::ip::tcp::socket &get_socket (void);
  bool first_operation_ended (void);
  bool is_ready_to_receive (void);
  bool is_persistant (void);
  void set_persistant (void);
  bool is_ready_to_send (void);

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
     \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/const_buffer.html">
          boost::asio::const_buffer</a>
   */
  std::unique_ptr<boost::asio::const_buffer> m_internal_send_buffer_ptr;
  //! Represents if errors occurred during the send operation.
  bool m_send_error = false;
  //! Represents if errors occurred during the receive operation.
  bool m_receive_error = false;
  //! Represents if the first operation has endend.
  /*!
     \sa first_operation_ended
    */
  bool m_first_operation_ended = false;
  //! Represents if it is possible to send bytes.
  bool m_ready_to_send = true;
  //! Represents if the connection is set to be persistant.
  bool m_persistant = false;
  // Functions
  void m_handle_send (const boost::system::error_code &,
                      size_t /*bytes_transferred*/);
  void m_handle_receive (const boost::system::error_code &, size_t);
};

//! Class constructor.
/*!
  \param executor <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/io_context.html">
                   boost::asio::io_context& executor</a>
*/
Connection::Connection (boost::asio::io_context &executor)
    : m_socket (executor)
{
}

//! Class destructor.
Connection::~Connection (void) {}

//! Completion handler, it is called when the send completes.
/*!
  \param error Result of operation.
  \param bytes_transferred Number of bytes received.
  \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_stream_socket/async_receive/overload1.html">
       async_send</a>
*/
void
Connection::m_handle_send (const boost::system::error_code &error,
                           size_t bytes_transferred)
{
  std::cerr << "INFO: Connection: user "
            << m_socket.remote_endpoint ().address ()
            << ": m_handle_send: bytes transferred = " << bytes_transferred
            << std::endl;
  if (!error)
    {
      m_internal_send_buffer_ptr.release ();
      m_first_operation_ended = true;
      m_ready_to_send = true;
    }
  else
    {
      m_send_error = true;
      std::cerr << "Error" << std::endl;
    }
}

//! Completion handler, it is called when the receive completes.
/*!
  \param error Result of operation.
  \param bytes_transferred Number of bytes received.
  \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_stream_socket/async_receive/overload1.html">
       async_receive</a>
*/
void
Connection::m_handle_receive (const boost::system::error_code &error,
                              size_t bytes_transferred)
{
  std::cerr << "INFO: Connection: user "
            << m_socket.remote_endpoint ().address ()
            << ": m_handle_receive: bytes transferred = " << bytes_transferred
            << std::endl;
  if (!error)
    {
      m_first_operation_ended = true;
    }
  else
    {
      m_receive_error = true;
      std::cerr << "Error" << std::endl;
    }
}

//! Asynchronous operation that writes a certain amount of data to a stream socket before completion.
/*!
  Works with the boost::asio::async_write function.
  \sa m_handle_send
  \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/async_write.html">
       async_write</a>
*/
void
Connection::send (void)
{
  boost::asio::async_write (
      m_socket, *m_internal_send_buffer_ptr,
      boost::bind (&Connection::m_handle_send, this,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred));
  m_ready_to_send = false;
}

//! This function is used to asynchronously receive data from the stream socket before completion.
/*!
  Works with the basic_stream_socket::async_receive function.
  \sa m_handle_receive
  \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_stream_socket/async_receive/overload1.html">
       async_receive</a>
*/
void
Connection::receive (void)
{
  m_internal_receive_buffer_ptr.reset (new boost::asio::mutable_buffer (
      new char[m_socket.available ()], m_socket.available ()));
  m_socket.async_receive (
      *m_internal_receive_buffer_ptr,
      boost::bind (&Connection::m_handle_receive, this,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred));
}

//! This function regulates the data loading.
/*!
  Works with const std::string as input.
  \param input data to be loaded.
*/
void
Connection::load_data (const std::string &input)
{
  m_internal_receive_buffer_ptr.release ();
  m_internal_send_buffer_ptr.reset (
      new boost::asio::const_buffer (input.data (), input.size ()));
}

//! This function regulates the data loading.
/*!
  Works with const std::vector<unsigned char> as input.
  \param input data to be loaded.
*/
void
Connection::load_data (const std::vector<unsigned char> &input)
{
  m_internal_receive_buffer_ptr.release ();
  m_internal_send_buffer_ptr.reset (
      new boost::asio::const_buffer (input.data (), input.size ()));
}

//! This function regulates the data loading.
/*!
  Works with boost::asio::const_buffer as input.
  \param input data to be loaded.
*/
void
Connection::load_data (const boost::asio::const_buffer &input)
{
  m_internal_receive_buffer_ptr.release ();
  m_internal_send_buffer_ptr.reset (
      new boost::asio::const_buffer (input.data (), input.size ()));
}

//! This function regulates the data unloading.
const std::shared_ptr<char[]>
Connection::unload_data (void) const
{
  size_t size = m_internal_receive_buffer_ptr->size ();
  std::shared_ptr<char[]> ret_ptr;
  ret_ptr.reset (new char[size]);
  ::memcpy (ret_ptr.get (), m_internal_receive_buffer_ptr->data (),
            m_internal_receive_buffer_ptr->size ());
  return ret_ptr;
}

//! Returns the private member m_socket.
/*!
  \sa m_socket
*/
boost::asio::ip::tcp::socket &
Connection::get_socket (void)
{
  return m_socket;
}

//! This function returns true if the first operation has endend.
bool
Connection::first_operation_ended (void)
{
  return m_first_operation_ended;
}

//! This function returns if the connection is ready to receive new data.
/*!
  Checks if the private member m_socket is ready for receive operation.
  \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/basic_stream_socket/available.html">
       basic_stream_socket::available</a>
*/
bool
Connection::is_ready_to_receive (void)
{
  if (m_socket.available () > 0)
    {
      return true;
    }
  else
    {
      return false;
    }
}

//! Sets a connection as persistant.
/*!
  A persistant connection is the idea of using a single TCP connection to send and
  receive multiple requests/responses, as opposed to opening/closing a new connection for 
  every single request/response pair.
  \sa m_persistant
*/
void
Connection::set_persistant (void)
{
  m_persistant = true;
}

//! Checks if the connection is persistant.
/*!
  \sa set_persistant
  \sa m_persistant
*/
bool
Connection::is_persistant (void)
{
  return m_persistant;
}

//! Checks if the connection is ready to send.
/*!
  m_ready_to_send is set false by the function send, and is set true again by the 
  m_handle_send function.
  \sa send
*/
bool
Connection::is_ready_to_send (void)
{
  return m_ready_to_send;
}

#endif

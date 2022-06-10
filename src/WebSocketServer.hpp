#ifndef WEBSOCKETSERVER_HPP
#define WEBSOCKETSERVER_HPP

#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <openssl/sha.h>
#include <stdexcept>
#include <string>

#include "Connection.hpp"
#include "TCPServer.hpp"

//! WebSocketServer defines a WebSocket providing full-duplex communication channels over a single TCP connection.
/*!
  This class communicates with the client side application written in JavaScript.
*/
class WebSocketServer : TCPServer
{
  // Functions
public:
  WebSocketServer (std::shared_ptr<boost::asio::io_context>, int);
  ~WebSocketServer (void);
  bool respond (void);
  bool full (void);
  void update_simulation_data (int, float, int, int);

private:
  std::string m_base64_encode (const std::string &);
  std::string m_base64_decode (const std::string &);
  std::string m_handshake_respond_builder (std::string);
  // format: eta velocity10 total
  std::vector<unsigned char> m_frame_builder (void);
  // Variables
  //! Implementation detail, used in function respond.
  /*!
    \sa respond
  */
  typedef enum
  {
    HANDSHAKE_ANSWARE,
    UPDATING_CLIENT,
    CLOSING_CONNECTION
  } m_status_t;
  //! Estimated time of arrival until the end of the simulation.
  unsigned int m_actual_eta;
  //! 10*velocity of the simulation (number of cycles per second).
  unsigned int m_actual_velocity10;
  //! Total number of steps performed during the simulation.
  unsigned int m_actual_total;
  //! Current step of the simulation.
  unsigned int m_actual_step;
  std::vector<unsigned char> m_actual_frame;
  //! This parameter represents if the client side application is connected to the socket.
  bool m_connected = false;
  /*!
    \sa m_status_t
  */
  m_status_t m_status;
  //! Implementation detail.
  /*!
    \sa respond
  */
  std::string m_handshake_response;
  //! Implementation detail.
  /*!
    \sa m_handshake_respond_builder
  */
  std::string m_handshake_request;
  //! Implementation detail.
  /*!
    \sa m_base64_encode
  */
  const char m_b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno"
                               "pqrstuvwxyz0123456789+/";
  //! Implementation detail.
  /*!
    \sa m_base64_decode
  */
  const char m_reverse_table[128]
      = { 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
          64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
          52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
          64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
          15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
          64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
          41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64 };
};
//! Class constructor.
/*!
  \param executor_ptr <a href="https://www.boost.org/doc/libs/master/doc/html/boost_asio/reference/io_context.html">boost::asio::io_context</a>
  \param port WebSocketServer port number.
*/
WebSocketServer::WebSocketServer (
    std::shared_ptr<boost::asio::io_context> executor_ptr, int port)
    : TCPServer (executor_ptr, port)
{
}
//! Class destructor.
WebSocketServer::~WebSocketServer (void) {}
//! This function supplies the correct response to the client depending on the connection status.
/*!
  Depending on m_status, there are 3 types of responses.
  \sa m_status_t
  - HANSHAKE_ANSWER: builds the handshake key using m_handshake_respond_builder
    and sends it to the client.
  \sa m_handshake_respond_builder
  - UPDATING_CLIENT: builds the data frame using m_frame-builder and sends it to the client.
  \sa m_frame_builder
  - CLOSING_CONNECTION: closes the connection.
*/
bool
WebSocketServer::respond (void)
{
  m_get_executor ().poll ();
  if (!m_is_waiting_list_empty ())
    {
      if (!m_connected)
        {
          m_status = HANDSHAKE_ANSWARE;
          m_connected = true;
        }
      switch (m_status)
        {
        case HANDSHAKE_ANSWARE:
          std::cerr << "INFO: WebSocketServer: respond: waiting for WS request"
                    << std::endl;
          while (m_get_first_connection ()
                     .connection_ptr->get_socket ()
                     .available ()
                 <= 0)
            {
            }
          std::cerr << "INFO: WebSocketServer: respond: recived WS request, "
                       "responding"
                    << std::endl;
          m_get_first_connection ().connection_ptr->receive ();
          m_handshake_response = "HTTP/1.1 101 Switching Protocols\r\n"
                                 "Upgrade: websocket\r\n"
                                 "Connection: Upgrade\r\n"
                                 "Sec-WebSocket-Accept: "
                                 + m_handshake_respond_builder (
                                     m_get_first_connection ()
                                         .connection_ptr->unload_data ()
                                         .get ())
                                 + "\r\n\r\n";
          m_get_first_connection ().connection_ptr->load_data (
              m_handshake_response);
          m_get_first_connection ().connection_ptr->send ();
          m_status = UPDATING_CLIENT;
          break;
        case UPDATING_CLIENT:
          if (m_get_first_connection ().connection_ptr->is_ready_to_send ())
            {
              m_actual_frame = m_frame_builder ();
              m_get_first_connection ().connection_ptr->load_data (
                  m_actual_frame);
              m_get_first_connection ().connection_ptr->send ();
            }
          if (m_actual_step >= m_actual_total)
            {
              m_status = CLOSING_CONNECTION;
            }
          break;
        case CLOSING_CONNECTION:
          if (m_get_first_connection ().connection_ptr->is_ready_to_send ())
            {
              m_delete_connection_by_index (0);
              return false;
            }
          break;
        }
    }
  return true;
}
//! This function returns if the WebSocketServer is full.
bool
WebSocketServer::full (void)
{
  return (!m_is_waiting_list_empty ());
}
//! This function encodes in base64.
/*!
  This is an outsourced library function.
  \param bindata Data to be encoded in base64.
*/
::std::string
WebSocketServer::m_base64_encode (const ::std::string &bindata)
{
  using ::std::numeric_limits;
  using ::std::string;

  if (bindata.size () > (numeric_limits<string::size_type>::max () / 4u) * 3u)
    {
      throw ::std::length_error ("Converting too large a string to base64.");
    }

  const ::std::size_t binlen = bindata.size ();
  // Use = signs so the end is properly padded.
  string retval ((((binlen + 2) / 3) * 4), '=');
  ::std::size_t outpos = 0;
  int bits_collected = 0;
  unsigned int accumulator = 0;
  const string::const_iterator binend = bindata.end ();

  for (string::const_iterator i = bindata.begin (); i != binend; ++i)
    {
      accumulator = (accumulator << 8) | (*i & 0xffu);
      bits_collected += 8;
      while (bits_collected >= 6)
        {
          bits_collected -= 6;
          retval[outpos++]
              = m_b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
  if (bits_collected > 0)
    { // Any trailing bits that are missing.
      assert (bits_collected < 6);
      accumulator <<= 6 - bits_collected;
      retval[outpos++] = m_b64_table[accumulator & 0x3fu];
    }
  assert (outpos >= (retval.size () - 2));
  assert (outpos <= retval.size ());
  return retval;
}
//! This function decodes from base64.
/*!
  This is an outsourced library function.
  \param ascdata Data to be decoded from base64.
*/
::std::string
WebSocketServer::m_base64_decode (const ::std::string &ascdata)
{
  using ::std::string;
  string retval;
  const string::const_iterator last = ascdata.end ();
  int bits_collected = 0;
  unsigned int accumulator = 0;

  for (string::const_iterator i = ascdata.begin (); i != last; ++i)
    {
      const int c = *i;
      if (::std::isspace (c) || c == '=')
        {
          // Skip whitespace and padding. Be liberal in what you accept.
          continue;
        }
      if ((c > 127) || (c < 0) || (m_reverse_table[c] > 63))
        {
          throw ::std::invalid_argument ("This contains characters not legal "
                                         "in a base64 encoded string.");
        }
      accumulator = (accumulator << 6) | m_reverse_table[c];
      bits_collected += 6;
      if (bits_collected >= 8)
        {
          bits_collected -= 8;
          retval
              += static_cast<char> ((accumulator >> bits_collected) & 0xffu);
        }
    }
  return retval;
}
//! This function builds a WebSocket handshake response.
/*!
  To establish a WebSocket connection, the client sends a WebSocket handshake
  request, for which the server returns a WebSocket handshake response.
*/
std::string
WebSocketServer::m_handshake_respond_builder (std::string req)
{
  m_handshake_request = req;
  std::string field ("Sec-WebSocket-Key: ");
  std::string magic_word ("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  std::string key (
      req.substr (req.find (field) + field.size (), 24)); //fixed lenght?
  std::cerr
      << "INFO: WebSocketServer: m_handshake_respond_builder: recived key "
      << key << std::endl;
  key += magic_word;
  unsigned char output[20];
  unsigned char key_c_str[key.size ()];
  ::memset (output, 0, 20);
  ::memcpy (key_c_str, key.c_str (), key.size ());
  ::SHA1 (key_c_str, key.size (), output);
  std::string s ((char *)output);
  s = m_base64_encode (s);
  std::cerr
      << "INFO: WebSocketServer: m_handshake_respond_builder: response key "
      << s << std::endl;
  return s;
}
//! This function builds a frame.
/*!
  The frame is built according to the following image:
  \image html frame.png width=481 height=457
  MASK is set to 1. Mask key is set to 0xe1e01cca
*/
//
std::vector<unsigned char>
WebSocketServer::m_frame_builder (void)
{
  std::vector<unsigned char> frame;
  std::vector<unsigned char> payload;
  std::vector<unsigned char> mask;
  mask.push_back (0xe1);
  mask.push_back (0xe0);
  mask.push_back (0x1c);
  mask.push_back (0xca);
  /*
   * FIN  = 1    set to 1 implies that every frame is seen as the last, since they are indipendently sent.
   * RSV1 = 0    using basic WebSocketServer.
   * RSV2 = 0
   * RSV3 = 0
   * OP C = 0    set to 0 implies that our datas will be read as binary.
   *    O = 0
   *    D = 1
   *    E = 0
  */
  frame.push_back (0b10000010);
  /*
   * MASK = 1
   * L    = 0
   * E    = 0
   * N    = 0
   * G    = 1
   * H    = 1
   * T    = 0
   * .    = 0
  */
  frame.push_back (0b10000000 + sizeof (m_actual_eta)
                   + sizeof (m_actual_velocity10) + sizeof (m_actual_total)
                   + sizeof (m_actual_step));
  /* mask key = 0xe1e01cca */
  frame.push_back (mask[0]);
  frame.push_back (mask[1]);
  frame.push_back (mask[2]);
  frame.push_back (mask[3]);
  /* payload eta */
  //dove c'è la FF tengo i bit orignali il resto viene settato a 0 e poi lo
  //sposto di x posizioni a destra per farlo diventare char (lavoro in bit >>)
  payload.push_back ((m_actual_eta & 0xFF000000) >> 24);
  payload.push_back ((m_actual_eta & 0x00FF0000) >> 16);
  payload.push_back ((m_actual_eta & 0x0000FF00) >> 8);
  payload.push_back ((m_actual_eta & 0x000000FF) >> 0);
  /* payload velocity10 */
  payload.push_back ((m_actual_velocity10 & 0xFF000000) >> 24);
  payload.push_back ((m_actual_velocity10 & 0x00FF0000) >> 16);
  payload.push_back ((m_actual_velocity10 & 0x0000FF00) >> 8);
  payload.push_back ((m_actual_velocity10 & 0x000000FF) >> 0);
  /* payload total */
  payload.push_back ((m_actual_total & 0xFF000000) >> 24);
  payload.push_back ((m_actual_total & 0x00FF0000) >> 16);
  payload.push_back ((m_actual_total & 0x0000FF00) >> 8);
  payload.push_back ((m_actual_total & 0x000000FF) >> 0);
  /* payload step */
  payload.push_back ((m_actual_step & 0xFF000000) >> 24);
  payload.push_back ((m_actual_step & 0x00FF0000) >> 16);
  payload.push_back ((m_actual_step & 0x0000FF00) >> 8);
  payload.push_back ((m_actual_step & 0x000000FF) >> 0);
  for (size_t i = 0; i < payload.size (); ++i)
    {
      frame.push_back (payload.at (i)
                       ^ mask[i % 4]); //apice è l'xor mask è vettore.
    }
  return frame;
}

//! This function updates the values of the time-parameters of the simulation.
/*!
  \param eta estimated time of arrival till the end of the simulation.
  \param velocity number of cycles performed per second.
  \param total total number of steps of the simulation.
  \param step current step of the simulation.
*/
void
WebSocketServer::update_simulation_data (int eta, float velocity, int total,
                                         int step)
{
  m_actual_eta = static_cast<unsigned int> (eta);
  m_actual_velocity10 = static_cast<unsigned int> (10 * velocity);
  m_actual_total = static_cast<unsigned int> (total);
  m_actual_step = static_cast<unsigned int> (step);
}

#endif

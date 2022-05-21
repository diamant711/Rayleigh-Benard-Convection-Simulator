///////////////////////////////////////////////////////////////////////////////
// 
// This class implement a WebSocket in order to comunicate with the JavaScript 
// client side application this class uses non-async operation in order to sync
// with the main program
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WEBSOCKETSERVER_HPP
#define WEBSOCKETSERVER_HPP

#include <iostream>
#include <memory>
#include <cstring>
#include <openssl/sha.h>
#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>

#include "TCPServer.hpp"
#include "Connection.hpp"

class WebSocketServer : TCPServer {
  // Functions
  public:
    WebSocketServer(boost::asio::io_context &, int);
    ~WebSocketServer(void);

    bool respond(void);
    bool full(void);
  private:
    std::string m_base64_encode(const std::string &);
    std::string m_base64_decode(const std::string &);
    std::string m_handshake_respond_builder(std::string);
    // format: eta 10velocity total
    std::vector<unsigned char> m_frame_builder(int eta, int 10velocity, int total);
  // Variables
  typedef enum {
    HANDSHAKE_ANSWARE,
    UPDATING_CLIENT,
    CLOSING_CONNECTION
  } m_status_t;
  bool m_connected = false;
  m_status_t m_status;
  std::string m_handshake_response;
  std::string m_handshake_request;
  std::unique_ptr<boost::asio::io_context> m_io_context_ptr;
  std::unique_ptr<Connection> m_connection_ptr;
  const char m_b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno"
                                    "pqrstuvwxyz0123456789+/";

  const char m_reverse_table[128] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
  };
};

WebSocketServer::WebSocketServer(boost::asio::io_context& executor, int port) 
  : TCPServer(executor, port) 
{}

WebSocketServer::~WebSocketServer(void) {}

bool WebSocketServer::respond(void) {
  m_get_executor().poll();
  if(!m_is_waiting_list_empty()) {
    if(!m_connected) {
      m_status = HANDSHAKE_ANSWARE;
      m_connected = true;
    }
    switch (m_status) {
      case HANDSHAKE_ANSWARE:
        std::cerr << "INFO: WebSocketServer: respond: waiting for WS request" 
                  << std::endl;
        while (m_get_first_connection().connection_ptr->get_socket().available() <= 0) {}
        std::cerr << "INFO: WebSocketServer: respond: recived WS request, responding" 
                  << std::endl;
        m_get_first_connection().connection_ptr->receive();
        m_handshake_response =  "HTTP/1.1 101 Switching Protocols\r\n"
                                "Upgrade: websocket\r\n"
                                "Connection: Upgrade\r\n"
                                "Sec-WebSocket-Accept: " 
                                + m_handshake_respond_builder(
                                  m_get_first_connection().connection_ptr->unload_data().get()
                                ) + "\r\n\r\n";
        m_get_first_connection().connection_ptr->load_data(m_handshake_response);
        m_get_first_connection().connection_ptr->send();
        m_status = UPDATING_CLIENT;
      break;
      case UPDATING_CLIENT:
      break;
      case CLOSING_CONNECTION:
        return false;
      break;
    }
  }
  return true;
}

bool WebSocketServer::full(void) {
  return (!m_is_waiting_list_empty());
}

::std::string WebSocketServer::m_base64_encode(const ::std::string &bindata)
{
   using ::std::string;
   using ::std::numeric_limits;

   if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
      throw ::std::length_error("Converting too large a string to base64.");
   }

   const ::std::size_t binlen = bindata.size();
   // Use = signs so the end is properly padded.
   string retval((((binlen + 2) / 3) * 4), '=');
   ::std::size_t outpos = 0;
   int bits_collected = 0;
   unsigned int accumulator = 0;
   const string::const_iterator binend = bindata.end();

   for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
      accumulator = (accumulator << 8) | (*i & 0xffu);
      bits_collected += 8;
      while (bits_collected >= 6) {
         bits_collected -= 6;
         retval[outpos++] = m_b64_table[(accumulator >> bits_collected) & 0x3fu];
      }
   }
   if (bits_collected > 0) { // Any trailing bits that are missing.
      assert(bits_collected < 6);
      accumulator <<= 6 - bits_collected;
      retval[outpos++] = m_b64_table[accumulator & 0x3fu];
   }
   assert(outpos >= (retval.size() - 2));
   assert(outpos <= retval.size());
   return retval;
}

::std::string WebSocketServer::m_base64_decode(const ::std::string &ascdata)
{
   using ::std::string;
   string retval;
   const string::const_iterator last = ascdata.end();
   int bits_collected = 0;
   unsigned int accumulator = 0;

   for (string::const_iterator i = ascdata.begin(); i != last; ++i) {
      const int c = *i;
      if (::std::isspace(c) || c == '=') {
         // Skip whitespace and padding. Be liberal in what you accept.
         continue;
      }
      if ((c > 127) || (c < 0) || (m_reverse_table[c] > 63)) {
         throw ::std::invalid_argument("This contains characters not legal in a base64 encoded string.");
      }
      accumulator = (accumulator << 6) | m_reverse_table[c];
      bits_collected += 6;
      if (bits_collected >= 8) {
         bits_collected -= 8;
         retval += static_cast<char>((accumulator >> bits_collected) & 0xffu);
      }
   }
   return retval;
}

std::string WebSocketServer::m_handshake_respond_builder(std::string req) {
  m_handshake_request = req;
  std::string field("Sec-WebSocket-Key: ");
  std::string magic_word("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  std::string key(req.substr(req.find(field) + field.size(), 24)); //fixed lenght?
  std::cerr << "INFO: WebSocketServer: m_handshake_respond_builder: recived key "
            << key << std::endl;
  key += magic_word;
  unsigned char output[20];
  unsigned char key_c_str[key.size()];
  ::memset(output, 0, 20);
  ::memcpy(key_c_str, key.c_str(), key.size());
  ::SHA1(key_c_str, key.size(), output);
  std::string s((char *) output);
  s = m_base64_encode(s);
  std::cerr << "INFO: WebSocketServer: m_handshake_respond_builder: response key "
            << s << std::endl;
  return s;
}

std::vector<unsigned char> WebServer::m_frame_builder(int eta, int velocity, int total) {
  std::vector<unsigned char> frame;
  /* 
   * FIN  = 1
   * RSV1 = 0
   * RSV2 = 0
   * RSV3 = 0
   * OP C = 0
   *    O = 0
   *    D = 1
   *    E = 0
  */
  frame.push_back(0b10000010);
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
  frame.push_back(0b10000000 + sizeof(eta) + sizeof(10velocity) + sizeof(total));
  /* mask key = 0xA271 */
  frame.push_back(0xA2);
  frame.push_back(0x71);
  /* payload eta */
  frame.push_back(eta & 0xFF000000 >> 6);
  frame.push_back(eta & 0x00FF0000 >> 4);
  frame.push_back(eta & 0x0000FF00 >> 2);
  frame.push_back(eta & 0x000000FF >> 0);
  /* payload 10velocity */
  frame.push_back(velocity & 0xFF000000 >> 6);
  frame.push_back(velocity & 0x00FF0000 >> 4);
  frame.push_back(velocity & 0x0000FF00 >> 2);
  frame.push_back(velocity & 0x000000FF >> 0);
  /* payload total */
  frame.push_back(total & 0xFF000000 >> 6);
  frame.push_back(total & 0x00FF0000 >> 4);
  frame.push_back(total & 0x0000FF00 >> 2);
  frame.push_back(total & 0x000000FF >> 0);
  return frame;
}

#endif

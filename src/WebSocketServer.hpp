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
#include <string>
#include <memory>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/compute/detail/sha1.hpp>

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
    std::string m_decode64(const std::string);
    std::string m_encode64(const std::string);
    std::string m_handshake_respond_builder(std::string);

  // Variables
  typedef enum {
    HANDSHAKE_ANSWARE,
    UPDATING_CLIENT,
    CLOSING_CONNECTION
  } m_status_t;
  bool m_connected = false;
  m_status_t m_status;
  std::unique_ptr<boost::asio::io_context> m_io_context_ptr;
  std::unique_ptr<Connection> m_connection_ptr;

};

WebSocketServer::WebSocketServer(boost::asio::io_context& executor, int port) 
  : TCPServer(executor, port) 
{}

WebSocketServer::~WebSocketServer(void) {}

bool WebSocketServer::respond(void) {
  m_get_executor().poll();
  if(!m_is_waiting_list_empty()) {
    std::cerr << "INFO: WebSocketServer: respond: there is a connection!" 
              << std::endl;
    if(!m_connected) {
      m_status = HANDSHAKE_ANSWARE;
      m_connected = true;
    }
    switch (m_status) {
      case HANDSHAKE_ANSWARE:
        std::cerr << "INFO: WebSocketServer: respond: waiting for WS request" << std::endl;
        while (m_get_first_connection().connection_ptr->get_socket().available() <= 0) {}
        std::cerr << "INFO: WebSocketServer: respond: recived WS request, responding..." << std::endl;
        m_get_first_connection().connection_ptr->receive();
        m_get_first_connection().connection_ptr->load_data(
          "HTTP/1.1 101 Switching Protocols\r\n"
          "Upgrade: websocket\r\n"
          "Connection: Upgrade\r\n"
          "Sec-WebSocket-Accept: " 
          + m_handshake_respond_builder(
            m_get_first_connection().connection_ptr->unload_data().get()
          ) + "\r\n"
        );
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


std::string WebSocketServer::m_decode64(const std::string val) {
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
        return c == '\0';
    });
}

std::string WebSocketServer::m_encode64(const std::string val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}

std::string WebSocketServer::m_handshake_respond_builder(std::string req) {
  std::string field("Sec-WebSocket-Key: ");
  std::string magic_word("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  std::string key(req.substr(req.find(field) + field.size(), 24)); //fixed lenght?
  key += magic_word;
  boost::compute::detail::sha1 sha1 { key };
  std::string s { sha1 };
  return m_encode64(s);
}

bool WebSocketServer::full(void) {
  return (!m_is_waiting_list_empty());
}

#endif

///////////////////////////////////////////////////////////////////////////////
// 
// This class implement a WebSocket in order to comunicate with the JavaScript 
// client side application this class uses non-async operation in order to sync
// with the main program
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WEBSOCKETSERVER_HPP
#define WEBSOCKETSERVER_HPP

#include <memory>
#include <string>

#include "TCPServer"
#include "Connection.hpp"

class WebSocketServer : TCPServer {
  // Functions
  public:
    WebSocketServer(boost::asio::io_context &);
    ~WebSocketServer(void);

    void respond(void);
  private:
    bool m_handshake_respond_builder(std::string &);

  // Variables
  typedef enum {
    HANDSHAKE_ANSWARE,
    UPDATING_CLIENT,
    CLOSING_CONNECTION
  } m_status_t;
  bool m_connected = false;
  m_status_t m_status;
  unique_ptr<boost::asio::io_context> m_io_context_ptr;
  unique_ptr<Connection> m_connection_ptr;

};

WebSocketServer::WebSocketServer(boost::asio::io_context& executor) {
  m_io_context_ptr.reset(&executor);
}

WebSocketServer::~WebSocketServer(void) {}

void WebSocketServer::respond(void) {
  m_get_executor().poll();
  if(!m_is_waiting_list_empty()) {
    if(!m_connected) {
      m_status = HANDSHAKE_ANSWARE;
      m_connected = true;
    }
    switch (m_status) {
      case HANDSHAKE_ANSWARE:
      break;
      case UPDATING_CLIENT:
      break;
      case CLOSING_CONNECTION:
      break;
    }
  }
}

bool WebSocketServer::m_handshake_respond_builder(std::string& req) {
  
}

#endif

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

#include "Connection.hpp"

class WebSocketServer {
  // Functions
  public:
    WebSocketServer(boost::asio::io_context &);
    ~WebSocketServer(void);

    void accept(void);
  private:
  
  // Variables
  unique_ptr<boost::asio::io_context> m_io_context_ptr;
  unique_ptr<Connection> m_connection_ptr;

};

WebSocketServer::WebSocketServer(boost::asio::io_context& tmp) {
  m_io_context_ptr.reset(&tmp);
}

WebSocketServer::~WebSocketServer(void) {}

void WebSocketServer::accept(void) {
  
}

#endif

#include <iostream>

#include <boost/asio.hpp>

#include "TCPServer.hpp"
#include "WebPage.hpp"

class WebServer : public TCPServer {
  public:
    WebServer(int);
    ~WebServer(void);
    void responde_to_all(void);
    bool is_first_user_connected(void);
  private:
    //Functions
    WebPage& m_generate_Output_page(void);
    //Variables
    WebPage m_Setup_page("cnt/Setup_page.html");
    WebPage m_Process_page("cnt/Process_page.html");
}

WebServer::WebServer(int port) : TCPServer(port) {}

WebServer::~WebServer() {}

bool is_first_user_connected(void) {
  return (m_connection_database.size() >= 1) ? true : false;
}

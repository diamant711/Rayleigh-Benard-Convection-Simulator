///////////////////////////////////////////////////////////////////////////////
//
//  WebServer.hpp specifies a class which define an asyncronus server that
//  serve web content
//
///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
 *
 * m_pages fixed indexing:
 *  1 - ServerFull page
 *  2 - Setup page
 *  3 - Process page
 *
 *****************************************************************************/


#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "TCPServer.hpp"
#include "WebPage.hpp"

class WebServer : public TCPServer {
  public:
    WebServer(int);
    ~WebServer(void);
    void respond_to_all(void);
  private:
    //Functions
    WebPage& m_generate_Output_page(void);
    //Variables
    std::vector<WebPage> m_pages;
    bool m_was_first_user_connected = false;
    boost::asio::ip::address m_first_user_address;
}

WebServer::WebServer(int port, 
                     std::string path_to_ServerFull_page, 
                     std::string path_to_Setup_page, 
                     std::string path_to_Process_page) : TCPServer(port) {
  m_pages.clear();
  m_pages.push_back(WebPage(path_to_ServerFull_page));
  m_pages.push_back(WebPage(path_to_Setup_page));
  m_pages.push_back(WebPage(path_to_Process_page));
}

WebServer::~WebServer() {}

void WebServer::respond_to_all(void) {
  if(!m_is_waiting_list_empty()) {
    if(!m_was_first_user_connected) {
      m_first_user_address = m_get_fist_connection()
                              .connection_ptr->remote_endpoint().address();
      m_was_first_user_connected = true;
    }
    for(int i = 0; i < m_get_plugged_connection(void); ++i) {
      if(m_get_connection_by_index(i)
          .connection_ptr->remote_endpoint().address() == m_first_user_address) {
        //struttura e memoria di stato
      } else {
        m_get_connection_by_index(i).connection_ptr
          ->load_data(m_pages.at(0).get_http_response());
        m_get_connection_by_index(i).connection_ptr->send();
      }
    }
  }  
  return;
}

#endif

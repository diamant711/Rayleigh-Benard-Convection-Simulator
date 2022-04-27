///////////////////////////////////////////////////////////////////////////////
//
//  WebServer.hpp specifies a class which define an asyncronus server that
//  serve web content
//
///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
 *
 * m_pages fixed indexing:
 *  0 - Error page
 *  1 - ServerFull page
 *  2 - Setup page
 *  3 - Process page
 *
 *****************************************************************************/
#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include <memory>
#include <sstream>

#include <boost/asio.hpp>

#include "TCPServer.hpp"
#include "WebPage.hpp"

class WebServer : public TCPServer {
  public:
    typedef enum {
      ERROR,
      NO_FIRST_USER,
      SETUP,
      PROCESSING,
      OUTPUT
    } first_user_status_t;
    typedef struct {
      unsigned int steps;
      double cold_temp;
      double hot_temp;
      double Ra;
      double Pr;
      double Re;
    } html_form_input_t;
    WebServer(int, std::string, std::string, std::string, std::string);
    ~WebServer(void);
    void respond_to_all(void);
    html_form_input_t get_user_input(void);
  private:
    //Functions
    WebPage& m_generate_Output_page(void);
    void m_gci_parser(const std::string &);
    //Variables
    WebServer::html_form_input_t m_internal_html_form_input;
    std::vector<std::unique_ptr<WebPage>> m_pages;
    bool m_was_first_user_connected = false;
    boost::asio::ip::address m_first_user_address;
    first_user_status_t m_first_user_status = NO_FIRST_USER;
};

WebServer::WebServer(int port, 
                     std::string path_to_Error_page, 
                     std::string path_to_ServerFull_page, 
                     std::string path_to_Setup_page, 
                     std::string path_to_Process_page) : TCPServer(port) {
  m_pages.clear();
  m_pages.push_back(std::unique_ptr<WebPage>(new WebPage(path_to_Error_page)));
  m_pages.push_back(std::unique_ptr<WebPage>(new WebPage(path_to_ServerFull_page)));
  m_pages.push_back(std::unique_ptr<WebPage>(new WebPage(path_to_Setup_page)));
  m_pages.push_back(std::unique_ptr<WebPage>(new WebPage(path_to_Process_page)));
}

WebServer::~WebServer() {}

void WebServer::respond_to_all(void) {
  if(!m_is_waiting_list_empty()) {
    for(size_t i = 0; i < m_get_plugged_connection(); ++i)
      if(m_get_connection_by_index(i).connection_ptr->first_operation_ended())
        m_delete_connection_by_index(i);

    if(!m_was_first_user_connected) {
      m_first_user_address = m_get_fist_connection().connection_ptr
                               ->get_socket().remote_endpoint().address();
      m_was_first_user_connected = true;
      m_first_user_status = SETUP;
    }

    for(size_t i = 0; i < m_get_plugged_connection(); ++i) {
      if(m_get_connection_by_index(i).connection_ptr
          ->get_socket().remote_endpoint().address() == m_first_user_address) {
        switch (m_first_user_status) {
          case SETUP:  
            m_get_connection_by_index(i).connection_ptr
              ->load_data(m_pages.at(2)->get_http_response());
            m_get_connection_by_index(i).connection_ptr->send();
            //CGI
          break;
          
          case PROCESSING:  
            m_get_connection_by_index(i).connection_ptr
              ->load_data(m_pages.at(3)->get_http_response());
            m_get_connection_by_index(i).connection_ptr->send();
            //PROGRESS BAR
          break;
          
          case OUTPUT:
            //raylib compilation and shipping
          break;
          
          case NO_FIRST_USER:  
          default:
            m_get_connection_by_index(i).connection_ptr
              ->load_data(m_pages.at(0)->get_http_response());
            m_get_connection_by_index(i).connection_ptr->send();
          break;
        }
      } else {
        m_get_connection_by_index(i).connection_ptr
          ->load_data(m_pages.at(1)->get_http_response());
        m_get_connection_by_index(i).connection_ptr->send();
      }
    }
  }  
  return;
}

WebServer::html_form_input_t WebServer::get_user_input(void) {
  return m_internal_html_form_input;
}

void WebServer::m_gci_parser(const std::string& http_request) {
  std::stringstream ss(http_request);
  std::string tmp_s;
  std::cout << "Start CGI analysis..." << std::endl;
  ss >> tmp_s;
  std::cout << "HTTP method: " << tmp_s << std::endl;
  ss >> tmp_s;

}

#endif

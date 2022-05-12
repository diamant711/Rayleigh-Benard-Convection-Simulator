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
      NO_FIRST_USER,
      ERROR,
      SETUP,
      CGI,
      PROCESSING,
      OUTPUT
    } first_user_status_t;
    typedef struct {
      unsigned int steps;
      double cwt;
      double hwt;
      double Ray;
      double Pr;
      double Rey;
    } html_form_input_t;
    WebServer(int, std::string, std::string, std::string, std::string);
    ~WebServer(void);
    void respond_to_all(void);
    html_form_input_t get_user_input(void);
    bool is_html_form_input_available(void);
  private:
    //Functions
    WebPage& m_generate_Output_page(void);
    void m_cgi_parser(const std::string &);
    //Variables
    const unsigned int m_n_input_parameter = 6;
    WebServer::html_form_input_t m_internal_html_form_input;
    std::vector<std::unique_ptr<WebPage>> m_pages;
    bool m_was_first_user_connected = false;
    boost::asio::ip::address m_first_user_address;
    first_user_status_t m_first_user_status = NO_FIRST_USER;
    bool m_cgi_parameter_available = false;
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
  for(size_t i = 0; i < m_get_plugged_connection(); ++i)
    if(m_get_connection_by_index(i).connection_ptr->first_operation_ended())
      m_delete_connection_by_index(i);

  if(!m_is_waiting_list_empty()) {
    std::cerr << "INFO: WebServer: respond_to_all: waiting list is not empty" 
              << std::endl;

    if(!m_was_first_user_connected) {
      m_first_user_address = m_get_fist_connection().connection_ptr
                               ->get_socket().remote_endpoint().address();
      m_was_first_user_connected = true;
      m_first_user_status = SETUP;
      std::cerr << "INFO: WebServer: respond_to_all: first user addres is " 
                << m_first_user_address << std::endl;
    }

    for(size_t i = 0; i < m_get_plugged_connection(); ++i) {
      if(m_get_connection_by_index(i).connection_ptr
          ->get_socket().remote_endpoint().address() == m_first_user_address) {
        switch (m_first_user_status) {
          case SETUP:  
            std::cerr << "INFO: WebServer: respond_to_all: first user at "
                      << "SETUP stage" << std::endl;
            m_get_connection_by_index(i).connection_ptr
              ->load_data(m_pages.at(2)->get_http_response());
            m_get_connection_by_index(i).connection_ptr->send();
            m_first_user_status = CGI;
            continue;
          break;

          case CGI:
            std::cerr << "INFO: WebServer: respond_to_all: first user at "
                      << "CGI stage" << std::endl;
            m_get_connection_by_index(i).connection_ptr->receive();
            m_cgi_parser(
              &(*m_get_connection_by_index(i).connection_ptr->unload_data())
            );
            m_first_user_status = PROCESSING;
            continue;
          break;
          
          case PROCESSING:  
            m_get_connection_by_index(i).connection_ptr
              ->load_data(m_pages.at(3)->get_http_response());
            m_get_connection_by_index(i).connection_ptr->send();
            //PROGRESS BAR
            continue;
          break;
          
          case OUTPUT:
            //raylib compilation and shipping
            continue;
          break;

          case ERROR:
          case NO_FIRST_USER:
            //something is wrong
            continue;
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

void WebServer::m_cgi_parser(const std::string& http_request) {
  // GET /?steps=3000&cwt=25&hwt=35&Ray=100&Pr=7&Rey=100 HTTP/1.1
  std::stringstream ss(http_request);
  std::string tmp_s;
  std::cerr << "INFO: WebServer: m_cgi_parser: Start CGI analysis..." << std::endl;
  ss >> tmp_s;
  std::cerr << "INFO: WebServer: m_cgi_parser: HTTP method used: " << tmp_s << std::endl;
  ss >> tmp_s;
  std::cerr << "INFO: WebServer: m_cgi_parser: CGI string = " << tmp_s << std::endl;
  tmp_s.erase(0, 2);
  for(unsigned int i = 0; i < m_n_input_parameter; ++i) {
    std::string token = tmp_s.substr(0, tmp_s.find_first_of("&"));
    size_t eq_pos = token.find_first_of("=");
    std::string p_name = token.substr(0, eq_pos);
    if(p_name == "steps") {
      m_internal_html_form_input.steps = 
        stoul(token.substr(eq_pos + 1, token.size()));   
      tmp_s.erase(0, token.size() + 1);
      std::cerr << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
                << p_name << "\',\t value = " 
                << stoul(token.substr(eq_pos + 1, token.size())) 
                << "}"  << std::endl;
      continue;
    } else if(p_name == "cwt") {
      m_internal_html_form_input.cwt = 
        stod(token.substr(eq_pos + 1, token.size()));  
      tmp_s.erase(0, token.size() + 1);
      std::cerr << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
                << p_name << "\',\t value = " 
                << stoul(token.substr(eq_pos + 1, token.size())) 
                << "}"  << std::endl;
      continue;
    } else if(p_name == "hwt") {
      m_internal_html_form_input.hwt = 
        stod(token.substr(eq_pos + 1, token.size()));   
      tmp_s.erase(0, token.size() + 1);
      std::cerr << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
                << p_name << "\',\t value = " 
                << stoul(token.substr(eq_pos + 1, token.size())) 
                << "}"  << std::endl;
      continue;
    } else if(p_name == "Ray") {
      m_internal_html_form_input.Ray = 
        stod(token.substr(eq_pos + 1, token.size()));   
      tmp_s.erase(0, token.size() + 1);
      std::cerr << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
                << p_name << "\',\t value = " 
                << stoul(token.substr(eq_pos + 1, token.size())) 
                << "}"  << std::endl;
      continue;
    } else if(p_name == "Pr") {
      m_internal_html_form_input.Pr = 
        stod(token.substr(eq_pos + 1, token.size()));   
      tmp_s.erase(0, token.size() + 1);
      std::cerr << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
                << p_name << "\',\t value = " 
                << stoul(token.substr(eq_pos + 1, token.size())) 
                << "}"  << std::endl;
      continue;
    } else if(p_name == "Rey") {
       m_internal_html_form_input.Rey = 
        stod(token.substr(eq_pos + 1, token.size()));   
      tmp_s.erase(0, token.size() + 1);
      std::cerr << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
                << p_name << "\',\t value = " 
                << stoul(token.substr(eq_pos + 1, token.size())) 
                << "}"  << std::endl;
      continue;
    } else {
      std::cerr << "WARNING: WebServer: m_gci_parser: Parameter name \'" << p_name 
                << "\' not recognized." << std::endl;
    }
  }
  m_cgi_parameter_available = true;
  std::cout << "INFO: WebServer: m_cgi_parser: Finished CGI analysis." << std::endl;
}

bool WebServer::is_html_form_input_available(void) {
  return m_cgi_parameter_available;
}

#endif

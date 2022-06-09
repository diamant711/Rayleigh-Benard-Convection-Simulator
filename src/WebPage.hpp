///////////////////////////////////////////////////////////////////////////////
//
//    WebPage: a class that hold and the body of the http request as long as
//             calculate the head of the request using the HTTP1.1 protocol
//
///////////////////////////////////////////////////////////////////////////////
//#############################################################################
//
//    HTTP1.1 response section:
//    1) status line example: "HTTP/1.1 200 OK\r\n"
//    2) response header file:
//      a) "Content-Type: text/html\r\n"
//      b) "Content-Lenght: <bytes-lenght of the body>\r\n"
//      c) ecc...
//    3) empty line "\r\n"
//    4) body (the copy of the html file)
//
//#############################################################################
/******************************************************************************
 *
 *    TODO:
 *      1) http file not found
 *      2) Ele's: fare assieme la descrizione della classe
 *
 *****************************************************************************/
#ifndef WEBPAGE_HPP
#define WEBPAGE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define HTTP_VERSION "HTTP/1.1"
//!
/*!
*/
class WebPage {
  public:
    //!
    typedef struct {
      unsigned int status_number;
      std::string status_phrase;
    } status_code_t;

    WebPage(const char *);
    WebPage(const std::string &);
    ~WebPage(void);

    const std::string& get_http_response(void) const;

  private:
    //Variables
    //!
    std::string m_file_path;
    //!
    std::ifstream m_input_file;
    //!
    std::string m_content_type;
    //!
    std::string m_http_header;
    //!
    std::unique_ptr<std::string> m_http_body_ptr;
    //!
    std::string m_http_response;
    //!
    std::string m_content_length;
    //!
    status_code_t m_status_code = {200, "OK"};
    //!
    bool m_ready_to_ship = false;
    //function
    const std::string m_get_extension_from_path(const std::string &);
    const std::string m_get_extension_from_path(const char *);
    bool m_fill_http_header(void);
    void m_compose_response(void);
    std::string m_itos(int);
};
//!
/*!

*/
std::string WebPage::m_itos(int n) {
  static const unsigned int max_number_digits = 100;
  std::string ret;
  char buf[max_number_digits];
  std::sprintf(buf, "%d", n);
  ret = buf;
  return ret;
}
//!
const std::string& WebPage::get_http_response(void) const {
  return m_http_response;
}
//TODO guardare assieme questa
//!
/*!

*/
bool WebPage::m_fill_http_header(void) {
  bool content_type_ok = true;
  std::string ext(m_get_extension_from_path(m_file_path));
  if(ext == "plain") {
    m_content_type = "text/plain";
  } else if(ext == "html") {
    m_content_type = "text/html";
  } else if(ext == "css") {
    m_content_type = "text/css";
  } else if(ext == "js") {
    m_content_type = "application/javascript";
  } else if(ext == "wasm") {
    m_content_type = "application/wasm";
  } else if(ext == "ico") {
    m_content_type = "image/x-icon";
  } else {
    std::cerr << "Warnings: Unknown file type deceted... disabling"
                 " Content-Type tag in http header..." << std::endl;
    content_type_ok = false;
  }
  if(m_input_file.good()) {
    std::string tmp;
    m_http_body_ptr->clear();
    while(!m_input_file.eof())
      m_http_body_ptr->push_back(m_input_file.get());
    m_http_body_ptr->pop_back(); //pop EOF char
    m_content_length = m_itos(m_http_body_ptr->size());
    tmp.clear();
    tmp = HTTP_VERSION;
    tmp += ' ';
    tmp += m_itos(m_status_code.status_number);
    tmp += ' ';
    tmp += m_status_code.status_phrase;
    tmp += '\r';
    tmp += '\n';
    if(content_type_ok) {
      tmp += "Content-Type: ";
      tmp += m_content_type;
      tmp += "; charset=utf-8";
      tmp += '\r';
      tmp += '\n';
    }
    tmp += "Content-Length: ";
    tmp += m_content_length;
    tmp += '\r';
    tmp += '\n';
    m_http_header = tmp;
    m_input_file.clear();
    m_input_file.seekg(0);
    return true;
  } else {
    std::cerr << "Error: file not good" << std::endl;
    return false;
  }
}
//!
/*!

*/
void WebPage::m_compose_response(void) {
  if(m_fill_http_header()) {
    m_http_response = m_http_header + "\r\n" + *m_http_body_ptr + "\r\n";
  } else {
    std::cerr << "ERROR: WebServer: m_compose_response: "
              << "Due to " << m_file_path
              << " source file errors the http response wont be completed"
              << std::endl;
  }
}
//!
/*!
\param path
*/
const std::string WebPage::m_get_extension_from_path(const std::string &path) {
  std::string::size_type start_index = path.find_last_of('.');
  if (start_index == path.size()) {
    return "plain";
  } else {
    return path.substr(start_index + 1, path.size() - start_index + 1);
  }
}
//!
/*!
\param path
*/
const std::string WebPage::m_get_extension_from_path(const char *path) {
  std::string tmp(path);
  tmp = m_get_extension_from_path(tmp);
  return tmp;
}
//TODO guardare anche questa
//! Class constructor.
WebPage::WebPage(const char *path) : m_file_path(path), m_input_file(path) {
  m_http_body_ptr.reset(new std::string(""));
  m_compose_response();
}

//TODO guardare assieme questa funzione
//! Class constructor.
WebPage::WebPage(const std::string &path) : m_file_path(path), m_input_file(path) {
  m_http_body_ptr.reset(new std::string(""));
  m_compose_response();
}
//! Class destructor.
WebPage::~WebPage(void) {}

#endif

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
 *      1) get_http_response method
 *      2) tests...
 *      3) http file not found
 *
 *****************************************************************************/

#include <vector>
#include <fstream>
#include <iostream>

#define MAX_BODY_SIZE_DIGITS 100
#define HTTP_VERSION "HTTP/1.1"

class WebPage {
  public:
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
    std::ifstream m_input_file;
    std::string m_content_type;
    std::string m_http_header;
    std::string m_http_body;
    std::string m_http_response;
    status_code_t m_status_code = {200, "OK\0"};
    const std::vector<std::string> m_content_types = {
      "text/html",
      "text/plain"
    };
    bool m_ready_to_ship = false;
    //function
    const std::string m_get_extension_from_path(const std::string &);
    const std::string m_get_extension_from_path(const char *);
    bool m_fill_http_header(void);
    bool m_fill_http_body(void);
    void m_compose_response(void);
};

bool WebPage::m_fill_http_header(void) {
  if(m_input_file.good()) {
    std::string tmp;
    m_input_file >> tmp;
    char content_lenght[MAX_BODY_SIZE_DIGITS];
    std::sprintf(content_lenght, "%d", tmp.size());
    tmp.clear();
    tmp = HTTP_VERSION + ' ' + m_status_code.status_number 
                       + ' ' + m_status_code.status_phrase + '\r' + '\n'
          + "Content-Type: " + m_content_type + '\r' + '\n'
          + "Content-Lenght: " + content_lenght + '\r' + '\n';
    m_http_header = tmp;
    m_input_file.clear();
    m_input_file.seekg(0);
    return true;
  } else {
    std::cerr << "Error: file not good" << std::endl;
    return false;
  }
}

bool WebPage::m_fill_http_body(void) {
  if(m_input_file.good()) {
    m_input_file >> m_http_body;
    m_input_file.close();
    return true;
  } else {
    std::cerr << "Error: file not good" << std::endl;
    return false;
  } 
}

void WebPage::m_compose_response(void) {
  if(m_fill_http_header() && m_fill_http_body()) {
    m_http_response = m_http_header + "\r\n" + m_http_body;
  } else {
    std::cerr << "Due to file errors the http response wont be completed" << std::endl;
  }
}

const std::string WebPage::m_get_extension_from_path(const std::string &path) {
  std::string::size_type start_index = path.find_last_of('.');
  return (start_index == path.size()) ? "plain"
          : path.substr(start_index + 1, path.size() - start_index + 1);
}

const std::string WebPage::m_get_extension_from_path(const char *path) {
  std::string tmp_path(path);
  return m_get_extension_from_path(tmp_path);
}

WebPage::WebPage(const char *path) : m_input_file(path),
                                     m_content_type(
                                       *std::find(
                                         m_content_types.cbegin(),
                                         m_content_types.cend(),
                                         "text/" + m_get_extension_from_path(path)
                                       )
                                     )
{
  m_compose_response();
}

WebPage::WebPage(const std::string &path) : m_input_file(path),
                                     m_content_type(
                                       *std::find(
                                         m_content_types.cbegin(),
                                         m_content_types.cend(),
                                         m_get_extension_from_path(path)
                                       )
                                     )
{
  m_compose_response();
}

WebPage::~WebPage(void) {}

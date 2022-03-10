///////////////////////////////////////////////////////////////////////////////
//    
//    WebPage: a class that hold and the body of the http request as long as
//             calculate the head of the request using the HTTP1.1 protocol
//
///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
 *    HTTP1.1 response section:
 *    1) status line example: "HTTP/1.1 200 OK\r\n"
 *    2) response header file:
 *      a) "Content-Type: text/html\r\n"
 *      b) "Content-Lenght: <bytes-lenght of the body>\r\n"
 *      c) ecc...
 *    3) empty line "\r\n"
 *    4) body (the copy of the html file)
 *
 *****************************************************************************/
#include <vector>
#include <fstream>

class WebPage {
  public:
    WebPage(const char *);
    WebPage(const std::string &);
    ~WebPage();
    const std::string& get_http_response(void) const;
  private:
    typedef struct {
      unsigned int error_number;
      std::string error_phrase;
    } error_code;
    const std::vector<error_code> error_codes = {
      {200, "OK"}
    };
    std::ifstream m_input_file;
    bool m_ready_to_ship = false;
    std::string m_http_response;
};

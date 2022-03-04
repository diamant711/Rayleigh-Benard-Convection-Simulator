#include <boost/asio.hpp>
#include <iostream>

class WebServer {
  public:
    typedef struct {
      //densità
    } url_gci_parser_type;
    WebServer();
    ~WebServer();
    void check_for_new_connection(void);
    const url_gci_parser_type& url_gci_parser(std::string &) const;
    bool send_html_page(std::string &);
    bool update_status_bar();
  private:
    //Variables
    std::fstream m_ContentIn;
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::acceptor m_acceptor(m_io_context, boost::asio::ip::tcp::endpoint(tcp::v4(), 8080));
    boost::asio::ip::tcp::socket m_socket(m_io_context);
    boost::asio::ip::tcp::endpoint m_client_connected;
    bool m_is_client_connected = false;
    //Function
}

WebServer::WebServer() {}

WebServer::~WebServer() {}

void check_for_new_connection(void) {
  m_acceptor.async_accept(m_socket);
  if(m_socket.is_open()) {
    try{
      m_client_connected = m_socket.remote_endpoint()
    } catch(const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
    m_is_client_connected = true;
  }
}

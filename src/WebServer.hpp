///////////////////////////////////////////////////////////////////////////////
//
//  WebServer.hpp is an asyncronus server that
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
 *  4 - raylib.html
 *  5 - raylib.js
 *  6 - raylib.wasm
 *
 *****************************************************************************/
#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include <boost/asio.hpp>

#include "TCPServer.hpp"
#include "WebPage.hpp"
#include "WebSocketServer.hpp"
//!
/*!

*/
class WebServer : public TCPServer
{
public:
  //!
  typedef enum
  {
    NO_FIRST_USER,
    ERROR,
    SETUP,
    CGI,
    PROCESSING,
    OUTPUT
  } first_user_status_t;
  //!
  /*!
  \param steps total number of steps
  \param cwt cold wall temperature
  \param hwt hot wall temperature
  \param Ray Rayleigh number
  \param Pr Prandtl number
  \param Rey Reynolds number
  */
  typedef struct
  {
    unsigned int steps;
    double cwt;
    double hwt;
    double Ray;
    double Pr;
    double Rey;
  } html_form_input_t;
  WebServer (std::shared_ptr<boost::asio::io_context>, int, int, std::string,
             std::string, std::string, std::string);
  ~WebServer (void);
  html_form_input_t get_user_input (void);
  void update_simulation_state (int, float, int, int);
  void waiting_and_assign_first_user (void);
  void serve_setup_page (void);
  void read_cgi_input (void);
  void serve_processing_page (void);
  void update_processing_page (void);
  void serve_output_page (void);
  void set_first_user_status (first_user_status_t);

private:
  //Functions
  void m_close_unused_connection (void);
  WebPage &m_generate_Output_page (void);
  void m_cgi_parser (const std::string &);
  char m_extract_raylib_request (std::string);
  //Variables
  //! Unique pointer at WebSocketServer
  std::unique_ptr<WebSocketServer> m_websocketserver_ptr;
  //!
  bool m_start_websocket = false;
  //! Estimated time of awaiting until the end of the simulation.
  int m_actual_eta = -1;
  //! Current velocity of the simulation in cycles/second.
  float m_actual_velocity = -1;
  //! Total number of steps of the simulation.
  int m_actual_total = -1;
  //! Current step of the simulation.
  int m_actual_step = -1;
  //! Number of input parameters.
  const unsigned int m_n_input_parameter = 6;
  //!
  WebServer::html_form_input_t m_internal_html_form_input;
  //! std vector of std unique pointers to WebPage.
  std::vector<std::unique_ptr<WebPage> > m_pages;
  //! It represents if there ever was a first user connected.
  bool m_was_first_user_connected = false;
  //! It represents the first user's address.
  /*!
  \sa <a href="https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/ip__address.html">boost::asio::ip::address</a>
  */
  boost::asio::ip::address m_first_user_address;
  //!
  first_user_status_t m_first_user_status = NO_FIRST_USER;
  //!
  bool m_cgi_parameter_available = false;
  //!
  bool m_raylib_compiled = false;
  //!
  bool m_output_pages_sent[3] = { false, false, false };
  //!
  unsigned short m_wasm_req_count = 0;
};
//! Class constructor.
/*!
\param executor_ptr
\param portW WebPage port number.
\param portWS WebSocketServer port number.
\param path_to_Error_page
\param path_to_ServerFull_page
\param path_to_Setup_page
\param path_to_Process_page
*/
WebServer::WebServer (std::shared_ptr<boost::asio::io_context> executor_ptr,
                      int portW, int portWS, std::string path_to_Error_page,
                      std::string path_to_ServerFull_page,
                      std::string path_to_Setup_page,
                      std::string path_to_Process_page)
    : TCPServer (executor_ptr, portW)
{
  m_websocketserver_ptr.reset (new WebSocketServer (executor_ptr, portWS));
  m_pages.clear ();
  m_pages.push_back (
      std::unique_ptr<WebPage> (new WebPage (path_to_Error_page)));
  m_pages.push_back (
      std::unique_ptr<WebPage> (new WebPage (path_to_ServerFull_page)));
  m_pages.push_back (
      std::unique_ptr<WebPage> (new WebPage (path_to_Setup_page)));
  m_pages.push_back (
      std::unique_ptr<WebPage> (new WebPage (path_to_Process_page)));
  m_pages.push_back (
      std::unique_ptr<WebPage> (new WebPage ("img/favicon.ico")));
}
//! Class destructor.
WebServer::~WebServer () {}
//! 
WebServer::html_form_input_t
WebServer::get_user_input (void)
{
  return m_internal_html_form_input;
}
//!
/*!
\param http_request
*/
void
WebServer::m_cgi_parser (const std::string &http_request)
{
  // GET /?steps=3000&cwt=25&hwt=35&Ray=100&Pr=7&Rey=100 HTTP/1.1
  std::stringstream ss (http_request);
  std::string tmp_s;
  std::cerr << "INFO: WebServer: m_cgi_parser: Start CGI analysis..."
            << std::endl;
  ss >> tmp_s;
  std::cerr << "INFO: WebServer: m_cgi_parser: HTTP method used: " << tmp_s
            << std::endl;
  ss >> tmp_s;
  std::cerr << "INFO: WebServer: m_cgi_parser: CGI string = " << tmp_s
            << std::endl;
  if (tmp_s[0] != '/' || tmp_s[1] != '?')
    {
      std::cerr << "WARNING: WebServer: m_cgi_parser: the request do not"
                << " contain a CGI string" << tmp_s << std::endl;
      return;
    }
  tmp_s.erase (0, 2);
  for (unsigned int i = 0; i < m_n_input_parameter; ++i)
    {
      std::string token = tmp_s.substr (0, tmp_s.find_first_of ("&"));
      size_t eq_pos = token.find_first_of ("=");
      std::string p_name = token.substr (0, eq_pos);
      if (p_name == "steps")
        {
          m_internal_html_form_input.steps
              = stoul (token.substr (eq_pos + 1, token.size ()));
          tmp_s.erase (0, token.size () + 1);
          std::cerr
              << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
              << p_name << "\',\t value = " << m_internal_html_form_input.steps
              << "}" << std::endl;
          continue;
        }
      else if (p_name == "cwt")
        {
          m_internal_html_form_input.cwt
              = stod (token.substr (eq_pos + 1, token.size ()));
          tmp_s.erase (0, token.size () + 1);
          std::cerr
              << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
              << p_name << "\',\t value = " << m_internal_html_form_input.cwt
              << "}" << std::endl;
          continue;
        }
      else if (p_name == "hwt")
        {
          m_internal_html_form_input.hwt
              = stod (token.substr (eq_pos + 1, token.size ()));
          tmp_s.erase (0, token.size () + 1);
          std::cerr
              << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
              << p_name << "\',\t value = " << m_internal_html_form_input.hwt
              << "}" << std::endl;
          continue;
        }
      else if (p_name == "Ray")
        {
          m_internal_html_form_input.Ray
              = stod (token.substr (eq_pos + 1, token.size ()));
          tmp_s.erase (0, token.size () + 1);
          std::cerr
              << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
              << p_name << "\',\t value = " << m_internal_html_form_input.Ray
              << "}" << std::endl;
          continue;
        }
      else if (p_name == "Pr")
        {
          m_internal_html_form_input.Pr
              = stod (token.substr (eq_pos + 1, token.size ()));
          tmp_s.erase (0, token.size () + 1);
          std::cerr
              << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
              << p_name << "\',\t value = " << m_internal_html_form_input.Pr
              << "}" << std::endl;
          continue;
        }
      else if (p_name == "Rey")
        {
          m_internal_html_form_input.Rey
              = stod (token.substr (eq_pos + 1, token.size ()));
          tmp_s.erase (0, token.size () + 1);
          std::cerr
              << "INFO: WebServer: m_cgi_parser: found parameter {name = \'"
              << p_name << "\',\t value = " << m_internal_html_form_input.Rey
              << "}" << std::endl;
          continue;
        }
      else
        {
          std::cerr << "WARNING: WebServer: m_gci_parser: Parameter name \'"
                    << p_name << "\' not recognized." << std::endl;
          return;
        }
    }
  m_cgi_parameter_available = true;
  std::cout << "INFO: WebServer: m_cgi_parser: Finished CGI analysis."
            << std::endl;
  return;
}
//! This function updates the simulation statse.
/*!
\param e Estimated time of awaiting untill end of the simulation
\param v Velocity of the simulation (cycles/s)
\param t Total number of steps
\param s Current step of the simulation
*/
void
WebServer::update_simulation_state (int e, float v, int t, int s)
{
  m_actual_eta = e;
  m_actual_velocity = v;
  m_actual_total = t;
  m_actual_step = s;
}
//! This function
/*!
\param input
*/
char
WebServer::m_extract_raylib_request (std::string input)
{
  return input[input.find ('.') + 1];
}
//! This function closes an unused connection.
void
WebServer::m_close_unused_connection (void)
{
  m_get_executor ().poll ();
  for (size_t i = 0; i < m_get_plugged_connection (); ++i)
    if (m_get_connection_by_index (i).connection_ptr->first_operation_ended ()
        && !m_get_connection_by_index (i).connection_ptr->is_persistant ()
        && m_get_connection_by_index (i).connection_ptr->is_ready_to_send ())
      m_delete_connection_by_index (i);
}
//! This function assigns a first user.
/*!
It performs two tests before assigning the first user.
It checks if there are waiting connections in the waiting list and
if there is already a first user connected.
*/
void
WebServer::waiting_and_assign_first_user (void)
{
  while (1)
    {
      m_get_executor ().poll ();
      m_close_unused_connection ();
      if (!m_is_waiting_list_empty ())
        {
          if (!m_was_first_user_connected)
            {
              m_first_user_address = m_get_first_connection ()
                                         .connection_ptr->get_socket ()
                                         .remote_endpoint ()
                                         .address ();
              m_was_first_user_connected = true;
              m_first_user_status = SETUP;
              std::cerr
                  << "INFO: WebServer: respond_to_all: first user addres is "
                  << m_first_user_address << std::endl;
              return;
            }
        }
    }
}
//! This function 
void
WebServer::serve_setup_page (void)
{
  while (1)
    {
      m_get_executor ().poll ();
      m_close_unused_connection ();
      if (!m_is_waiting_list_empty ())
        {
          for (size_t i = 0; i < m_get_plugged_connection (); ++i)
            {
              if (m_get_connection_by_index (i)
                      .connection_ptr->get_socket ()
                      .remote_endpoint ()
                      .address ()
                  == m_first_user_address)
                {
                  if (m_first_user_status == SETUP)
                    {
                      if (m_get_connection_by_index (i)
                              .connection_ptr->is_ready_to_send ())
                        {
                          std::cerr << "INFO: WebServer: respond_to_all: "
                                       "first user at "
                                    << "SETUP stage" << std::endl;
                          m_get_connection_by_index (i)
                              .connection_ptr->load_data (
                                  m_pages.at (2)->get_http_response ());
                          m_get_connection_by_index (i)
                              .connection_ptr->send ();
                          m_get_connection_by_index (i)
                              .connection_ptr->load_data (
                                  m_pages.at (4)->get_http_response ());
                          m_get_connection_by_index (i)
                              .connection_ptr->send ();
                          m_first_user_status = CGI;
                          return;
                        }
                    }
                }
            }
        }
    }
}

void
WebServer::read_cgi_input (void)
{
  while (1)
    {
      m_get_executor ().poll ();
      m_close_unused_connection ();
      if (!m_is_waiting_list_empty ())
        {
          for (size_t i = 0; i < m_get_plugged_connection (); ++i)
            {
              if (m_get_connection_by_index (i)
                      .connection_ptr->get_socket ()
                      .remote_endpoint ()
                      .address ()
                  == m_first_user_address)
                {
                  if (m_first_user_status == CGI)
                    {
                      if (m_get_connection_by_index (i)
                              .connection_ptr->is_ready_to_receive ())
                        {
                          std::cerr << "INFO: WebServer: respond_to_all: "
                                       "first user at "
                                    << "CGI stage" << std::endl;
                          m_get_connection_by_index (i)
                              .connection_ptr->receive ();
                          m_cgi_parser (m_get_connection_by_index (i)
                                            .connection_ptr->unload_data ()
                                            .get ());
                          if (m_cgi_parameter_available)
                            {
                              m_first_user_status = PROCESSING;
                              return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void
WebServer::serve_processing_page (void)
{
  while (1)
    {
      m_get_executor ().poll ();
      m_close_unused_connection ();
      if (!m_is_waiting_list_empty ())
        {
          for (size_t i = 0; i < m_get_plugged_connection (); ++i)
            {
              if (m_get_connection_by_index (i)
                      .connection_ptr->get_socket ()
                      .remote_endpoint ()
                      .address ()
                  == m_first_user_address)
                {
                  if (m_first_user_status == PROCESSING)
                    {
                      if (m_get_connection_by_index (i)
                              .connection_ptr->is_ready_to_send ())
                        {
                          if (m_start_websocket == false)
                            {
                              std::cerr << "INFO: WebServer: respond_to_all: "
                                           "first user at "
                                        << "PROCESSING stage" << std::endl;
                              m_get_connection_by_index (i)
                                  .connection_ptr->load_data (
                                      m_pages.at (3)->get_http_response ());
                              m_get_connection_by_index (i)
                                  .connection_ptr->send ();
                              m_get_connection_by_index (i)
                                  .connection_ptr->load_data (
                                      m_pages.at (4)->get_http_response ());
                              m_get_connection_by_index (i)
                                  .connection_ptr->send ();
                              m_start_websocket = true;
                              return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void
WebServer::update_processing_page (void)
{
  if (m_start_websocket)
    {
      m_websocketserver_ptr->update_simulation_data (
          m_actual_eta, m_actual_velocity, m_actual_total, m_actual_step);
      m_start_websocket = m_websocketserver_ptr->respond ();
      m_first_user_status = m_start_websocket ? PROCESSING : OUTPUT;
    }
}

void
WebServer::serve_output_page (void)
{
  while (1)
    {
      m_get_executor ().poll ();
      m_close_unused_connection ();
      if (!m_is_waiting_list_empty ())
        {
          for (size_t i = 0; i < m_get_plugged_connection (); ++i)
            {
              if (m_get_connection_by_index (i)
                      .connection_ptr->get_socket ()
                      .remote_endpoint ()
                      .address ()
                  == m_first_user_address)
                {
                  if (m_first_user_status == OUTPUT)
                    {
                      if (!m_raylib_compiled)
                        {
                          std::cerr << "INFO: WebServer: respond_to_all: "
                                       "first user at "
                                    << "OUTPUT stage" << std::endl;
                          if (::system ("make raylib") != 0)
                            {
                              std::cerr
                                  << "ERROR: WebServer: serve_output_page: "
                                  << "failed raylib for web compilation. "
                                  << "To resolve and see output execute 'make "
                                     "purge && "
                                  << "make && "
                                     "Rayleigh-Benard-Convection-Simulator "
                                     "8080 "
                                  << "8000 --output_only'" << std::endl;
                              ::exit (1);
                            }
                          m_pages.push_back (std::unique_ptr<WebPage> (
                              new WebPage ("cnt/raylib.html")));
                          m_pages.push_back (std::unique_ptr<WebPage> (
                              new WebPage ("cnt/raylib.js")));
                          m_pages.push_back (std::unique_ptr<WebPage> (
                              new WebPage ("cnt/raylib.wasm")));

                          m_raylib_compiled = true;
                        }
                      if (m_get_connection_by_index (i)
                              .connection_ptr->is_ready_to_send ()
                          && m_raylib_compiled)
                        {
                          if (m_get_connection_by_index (i)
                                  .connection_ptr->is_ready_to_receive ())
                            {
                              m_get_connection_by_index (i)
                                  .connection_ptr->receive ();
                              switch (m_extract_raylib_request (
                                  m_get_connection_by_index (i)
                                      .connection_ptr->unload_data ()
                                      .get ()))
                                {
                                case 'h': //html
                                  m_get_connection_by_index (i)
                                      .connection_ptr->load_data (
                                          m_pages.at (5)
                                              ->get_http_response ());
                                  m_get_connection_by_index (i)
                                      .connection_ptr->send ();
                                  m_output_pages_sent[0] = true;
                                  break;

                                case 'j': //js
                                  m_get_connection_by_index (i)
                                      .connection_ptr->load_data (
                                          m_pages.at (6)
                                              ->get_http_response ());
                                  m_get_connection_by_index (i)
                                      .connection_ptr->send ();
                                  m_output_pages_sent[1] = true;
                                  break;

                                case 'w': //wasm
                                  m_get_connection_by_index (i)
                                      .connection_ptr->load_data (
                                          m_pages.at (7)
                                              ->get_http_response ());
                                  m_get_connection_by_index (i)
                                      .connection_ptr->send ();
                                  m_wasm_req_count += 1;
                                  m_output_pages_sent[2] = true;
                                  break;

                                case 'i': //ico
                                  m_get_connection_by_index (i)
                                      .connection_ptr->load_data (
                                          m_pages.at (4)
                                              ->get_http_response ());
                                  m_get_connection_by_index (i)
                                      .connection_ptr->send ();
                                  break;

                                default:
                                  std::cerr
                                      << "WARNING: WebServer: respond_to_all: "
                                      << " possible error in the raylib "
                                         "request parser"
                                      << std::endl;
                                  break;
                                }
                            }
                        }
                    }
                }
            }
        }
      else if ((m_wasm_req_count == 2) && m_output_pages_sent[0]
               && m_output_pages_sent[1] && m_output_pages_sent[2])
        {
          return;
        }
    }
}

void
WebServer::set_first_user_status (first_user_status_t first_user_status)
{
  m_first_user_status = first_user_status;
}

#endif

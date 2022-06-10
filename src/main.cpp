#include <iostream>

#include "Connection.hpp"
#include "RayBenConvection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
#include "WebPage.hpp"
#include "WebServer.hpp"

int
main (int argc, char *argv[])
{
  char portW[10] = "8080";
  char portWS[10] = "8000";
  bool output_only = false;
  switch (argc)
    {
    case 4:
      if ((::atoi (argv[1]) >= 1) && (::atoi (argv[1]) <= 65535))
        ::strncpy (portW, argv[1], 5);
      else
        std::cerr << "WARNING: main: invalid WebServer port (" << argv[1]
                  << ") unsing defaults" << std::endl;
      if ((::atoi (argv[2]) >= 1) && (::atoi (argv[2]) <= 65535))
        ::strncpy (portWS, argv[2], 5);
      else
        std::cerr << "WARNING: main: invalid WebServerSocket port (" << argv[2]
                  << ") unsing defaults" << std::endl;
      if (::strcmp (argv[3], "--output_only") == 0)
        output_only = true;
      else
        std::cerr << "WARNING: main: unrecognized option (" << argv[3] << ")"
                  << std::endl;
      break;
    case 3:
      if ((::atoi (argv[1]) >= 1) && (::atoi (argv[1]) <= 65535))
        ::strncpy (portW, argv[1], 5);
      else
        std::cerr << "WARNING: main: invalid WebServer port (" << argv[1]
                  << ") unsing defaults" << std::endl;
      if ((::atoi (argv[2]) >= 1) && (::atoi (argv[2]) <= 65535))
        ::strncpy (portWS, argv[2], 5);
      else
        std::cerr << "WARNING: main: invalid WebServerSocket port (" << argv[2]
                  << ") unsing defaults" << std::endl;
      break;
    case 2:
      std::cerr << "WARNING: main: usage: " << argv[0] << " " << argv[1]
                << " [port websocketserver] [--output_only]" << std::endl
                << "WARNING: main: Using 8000 for WebSocketServer"
                << std::endl;
      break;
    case 1:
      std::cerr << "WARNING: main: usage: " << argv[0]
                << " [webserver port] [websocketserver port] [--output_only]"
                << std::endl
                << "WARNING: main: Using 8080 for WebServer and "
                << "8000 for WebSocketServer" << std::endl;
      break;
    }

  RayBenConvection RayBenCon;
  RayBenConvection::simulation_state_t simulation_state;
  std::shared_ptr<boost::asio::io_context> io_context_ptr (
      new boost::asio::io_context ());
  WebServer webserver (io_context_ptr, ::atoi (portW), ::atoi (portWS),
                       "cnt/Error_page.html", "cnt/ServerFull_page.html",
                       "cnt/Setup_page.html", "cnt/Process_page.html");

  webserver.waiting_and_assign_first_user ();

  if (!output_only)
    {
      webserver.serve_setup_page ();

      webserver.read_cgi_input ();

      WebServer::html_form_input_t html_form_input (
          webserver.get_user_input ());

      RayBenCon.init (html_form_input.steps, html_form_input.cwt,
                      html_form_input.hwt, html_form_input.Ray,
                      html_form_input.Pr, html_form_input.Rey);

      RayBenCon.write_current_data ();

      webserver.serve_processing_page ();

      // Process stage
      do
        {
          simulation_state = RayBenCon.eval_next_frame ();

          webserver.update_simulation_state (
              simulation_state.eta, simulation_state.velocity,
              simulation_state.total, simulation_state.step);

          RayBenCon.write_current_data ();

          webserver.update_processing_page ();
        }
      while (!simulation_state.ended);
    }
  else
    {
      webserver.set_first_user_status (WebServer::OUTPUT);
    }

  // Output stage
  webserver.serve_output_page ();

  std::cerr << "INFO: main: Program ended without errors" << std::endl;
  return 0;
}

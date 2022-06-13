#include <iostream>

#include "Connection.hpp"
#include "RayBenConvection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
#include "WebPage.hpp"
#include "WebServer.hpp"

void print_help_page (void);

int
main (int argc, char **argv)
{
  const unsigned short port_len = 5;
  const unsigned short port_min = 1;
  const unsigned short port_max = 65535;
  char portW[port_len] = "8080";
  char portWS[port_len] = "8000";
  char tmp[port_len];
  bool output_only = false;
  for (int i = 1; i < argc; ++i)
    {
      if (argv[i][0] == '-')
        {
          switch (argv[i][1])
            {
            case 's':
              ::strncpy (tmp, &argv[i][2], port_len);
              if ((::atoi (tmp) >= port_min) && (::atoi (tmp) <= port_max))
                {
                  ::strncpy (portWS, tmp, port_len);
                }
              break;
            case 'w':
              ::strncpy (tmp, &argv[i][2], port_len);
              if ((::atoi (tmp) >= port_min) && (::atoi (tmp) <= port_max))
                {
                  ::strncpy (portW, tmp, port_len);
                }
              break;
            case 'o':
              output_only = true;
              break;
            case 'h':
              print_help_page ();
              return 0;
              break;
            default:
              std::cerr << "WARNING: main: unrecognized option \'" << argv[i]
                        << "\'" << std::endl;
              break;
            }
        }
      else
        {
          std::cerr << "WARNING: main: unrecognized option \'" << argv[i]
                    << "\'" << std::endl;
        }
    }

  RayBenConvection RayBenCon;
  RayBenConvection::simulation_state_t simulation_state;
  std::shared_ptr<boost::asio::io_context> io_context_ptr (
      new boost::asio::io_context ());
  WebServer webserver (io_context_ptr, ::atoi (portW), ::atoi (portWS),
                       "cnt/Error_page.html", "cnt/ServerFull_page.html",
                       "cnt/Setup_page.html", "cnt/Process_page.html");

  std::cerr << "INFO: main: open this link \'http://localhost:";
  if (!output_only)
    std::cerr << portW << "\'" << std::endl;
  else
    std::cerr << portW << "/raylib.html\'" << std::endl;

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

void
print_help_page (void)
{
  std::cout
      << "Rayleigh Benard Convection Simulator" << std::endl
      << std::endl
      << "usage: Rayleigh-Benard-Convection-Simulator [arguments]" << std::endl
      << std::endl
      << "Arguments:" << std::endl
      << "\t-w <PORT>\t select different port for the web server" << std::endl
      << "\t-s <PORT>\t select different port for the websocket server"
      << std::endl
      << "\t-h\t\t show this help page" << std::endl
      << "\t-o\t\t permorm only the output phase" << std::endl
      << std::endl
      << "Note: if no arguments are provided the following call is performed"
      << std::endl
      << "\tRayleigh-Benard-Convection-Simulator -w8080 -s8000" << std::endl
      << std::endl
      << "Documentation at: doc/html/index.html" << std::endl
      << std::endl;
  return;
}

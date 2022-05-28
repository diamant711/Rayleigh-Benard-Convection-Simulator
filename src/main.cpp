#include <iostream>

#include "RayBenConvection.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
#include "WebServer.hpp"
#include "WebPage.hpp"

int main(int argc, char *argv[]){
  char portW[10]  = "8080";
  char portWS[10] = "8000";
  if(argc < 2) {
    std::cerr << "WARNING: main: usage: " << argv[0] << " [port webserver] [port websocketserver]." << std::endl
              << "WARNING: main: Using 8080 for WebServer and 8000 for WebSocketServer" << std::endl;
  } else if (argc == 2) {
    std::cerr << "WARNING: main: usage: " << argv[0] << " " << argv[1] << " [port websocketserver]." << std::endl
              << "WARNING: main: Using 8000 for WebSocketServer" << std::endl;
  } else {
    ::strncpy(portW,  argv[1], 5);
    ::strncpy(portWS, argv[2], 5);
  }
  
  RayBenConvection RayBenCon;
  std::shared_ptr<boost::asio::io_context> io_context_ptr(new boost::asio::io_context());
  WebServer webserver(io_context_ptr,
                      ::atoi(portW), ::atoi(portWS),
                      "cnt/Error_page.html", 
                      "cnt/ServerFull_page.html", 
                      "cnt/Setup_page.html",
                      "cnt/Process_page.html");
  RayBenConvection::simulation_state_t simulation_state;
  
  while(!webserver.is_html_form_input_available()) {
    webserver.respond_to_all();
  }
  
  webserver.respond_to_all();
  WebServer::html_form_input_t html_form_input(webserver.get_user_input());
  RayBenCon.init(html_form_input.steps,
                 html_form_input.cwt,
                 html_form_input.hwt,
                 html_form_input.Ray,
                 html_form_input.Pr,
                 html_form_input.Rey
  );
  RayBenCon.write_current_data();
  
  do {
    webserver.respond_to_all();
    RayBenCon.write_current_data();
    simulation_state = RayBenCon.eval_next_frame();
    webserver.update_simulation_state(
      simulation_state.eta,
      simulation_state.velocity,
      simulation_state.total,
      simulation_state.step
    );
  } while(!simulation_state.ended);

  return 0;
}

#include <iostream>

#include "RayBenConvection.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
#include "WebServer.hpp"
#include "WebPage.hpp"

int main(int argc, char *argv[]){
  char port[10] = "8080";
  if(argc < 2) {
    std::cerr << "WARNING: main: usage: " << argv[0] << " [port]. " 
              << "Using 8080" << std::endl;
  } else {
    ::strncpy(port, argv[1], 5);
  }
  
  RayBenConvection RayBenCon;
  boost::asio::io_context io_context;
  WebServer webserver(io_context, ::atoi(port), "cnt/Error_page.html", 
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
    simulation_state = RayBenCon.eval_next_frame();
    webserver.update_simulation_state(
      simulation_state.step,
      simulation_state.velocity,
      simulation_state.eta
    );
    RayBenCon.write_current_data();
  } while(!simulation_state.ended);

  webserver.poll();

  return 0;
}

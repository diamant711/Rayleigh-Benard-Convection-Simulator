#include "RayBenConvection.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
#include "WebServer.hpp"
#include "WebPage.hpp"

int main(int argc, char *argv[]){
  char port[5] = "8080";
  if(argc < 2) {
    std::cerr << "WARNING: main: usage: " << argv[0] << " [port]. " 
              << "Using 8080" << std::endl;
  } else {
    ::strncpy(port, argv[1], 5);
  }
  
  RayBenConvection RayBenCon;
  WebServer webserver(::atoi(port), "cnt/Error_page.html", 
                                    "cnt/ServerFull_page.html", 
                                    "cnt/Setup_page.html",
                                    "cnt/Process_page.html");
  while(!webserver.is_html_form_input_available()) {
    webserver.run_one();
    webserver.respond_to_all();
  }
  
  WebServer::html_form_input_t html_form_input(webserver.get_user_input());
  RayBenCon.init(html_form_input.steps,
                 html_form_input.cwt,
                 html_form_input.hwt,
                 html_form_input.Ray,
                 html_form_input.Pr,
                 html_form_input.Rey
  );
  
  for(int i = 0; ; ++i){
    std::cerr << "INFO: main: processing...\t(" << i << "/" 
              << html_form_input.steps << ")" << std::endl;
    webserver.run_one();
    RayBenCon.write_current_data();
    if (RayBenCon.eval_next_frame())
      break;
  }

  return 0;
}

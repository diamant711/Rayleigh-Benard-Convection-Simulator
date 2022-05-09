#include <iostream>

#include "RayBenConvection.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
#include "WebServer.hpp"
#include "WebPage.hpp"

int main(){
  /*
  RayBenConvection con;
  con.init(100, 25, 35, 150, 9, 130);

  while(1){
    con.write_current_data();
    if (con.eval_next_frame())
      break;
  }
  */
  
  WebServer webserver(8080, "cnt/Error_page.html", 
                            "cnt/ServerFull_page.html", 
                            "cnt/Setup_page.html",
                            "cnt/Process_page.html");
  while(1) {
    webserver.run_one();
    webserver.respond_to_all();
  }

  return 0;
}

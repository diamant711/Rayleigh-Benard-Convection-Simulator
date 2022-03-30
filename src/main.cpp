#include <iostream>

#include "RayBenConvection.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "TCPServer.hpp"
//#include "WebServer.hpp"
#include "WebPage.hpp"

int main(){
 
  RayBenConvection con;
  con.init(100, 25, 35, 150, 9, 130);

  while(1){
    con.write_current_data();
    if (con.eval_next_frame())
      break;
  }
  
  std::cout << "End main return 0" << std::endl;
  return 0;
}

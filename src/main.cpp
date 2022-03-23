#include "RayBenConvection.hpp"
//#include "Connection.hpp"
//#include "Server.hpp"
#include "WebPage.hpp"

int main(){
 
  RayBenConvection con;
  con.init(6000, 25, 35, 150, 9, 130);

  while(1){
    con.write_current_data();
    if (con.eval_next_frame())
      break;
  }
 
  return 0;
}

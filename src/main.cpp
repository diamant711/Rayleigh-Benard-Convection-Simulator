#include "RayBenConvection.hpp"
//#include "Connection.hpp"
//#include "Server.hpp"
#include "WebPage.hpp"

int main(){
 
  RayBenConvection con;
  con.init(25, 55, 1e2, 7, 100);

  while(1){
    con.write_current_data();
    if (con.eval_next_frame())
      break;
  }
 
  return 0;
}

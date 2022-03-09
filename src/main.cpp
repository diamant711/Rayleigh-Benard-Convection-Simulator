#include "RayBenConvection.hpp"
//#include "Server.hpp"

int main(){
  RayBenConvection con;
  con.init();

  while(1){
  con.write_current_data();
  if (con.eval_next_frame())
   break;
  }

  return 0;
}

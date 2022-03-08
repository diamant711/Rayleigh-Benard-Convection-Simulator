#include "RayBenConvection.hpp"

int main(){
  RayBenConvection con;
  con.init();

  while(!con.eval_next_frame()) {}

  return 0;
}

#include "RayBenConvection.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "WebPage.hpp"

int main(){
  /*
  RayBenConvection con;
  con.init();

  while(1){
    con.write_current_data();
    if (con.eval_next_frame())
      break;
  }
  */
  WebPage test_page("../cnt/test.html");
  std::cout << test_page.get_http_response() << std::endl;

  return 0;
}

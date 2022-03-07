#include "RayBenConvection.hpp"

int main(){
RayBenConvection con;
con.init();

for (int i=0; i<=5000; i++)
con.eval_next_frame();

return 0;
}

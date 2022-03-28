#include "temperature_matrix.h"
#include "../inc/raylib.h"
#include "jet.h"

#define PIXEL 4
#define WINDOW_WIDTH ((PIXEL*nx) + 3*nx)
#define WINDOW_HEIGHT ((PIXEL*ny) + 3*ny)
#define FPS 10

Color TtoC (double cold, double hot,  double t) {
   /*
   (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
   */
   if ((t <= hot) && (t >= cold)) {
     int index = (t - cold) * (JETSIZE - 1) / ( hot - cold );
     return jet[index];
   } else {
     return (Color){0, 0, 0, 0};
   }
}

void Draw(double cold, double hot, const double t[ny][nx])
  {
    BeginDrawing();
      ClearBackground((Color){ 255, 255, 255, 255});
      for(unsigned int i = 0; i < nx; ++i) {
        for(unsigned int k = 0; k < ny; ++k) {
          DrawRectangle((WINDOW_WIDTH/2 - nx*PIXEL/2) + i*PIXEL, 
                         (WINDOW_HEIGHT/2 + ny*PIXEL/2) - k*PIXEL, 
                         PIXEL, PIXEL, TtoC(cold, hot, t[i][k]));
        }
      }
  int posY = 5;
    for(unsigned int i = 0; i < JETSIZE; ++i) {
      DrawRectangle(5, posY, 5, 1, jet[i]);
     posY += 1;
   }
 EndDrawing();
}

int main(){

SetTargetFPS(FPS);

InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rayleigh-Benard Convection");

  for (int i = 0; i < n_steps; i++)
    Draw(cold_temp, hot_temp, T[i]);


CloseWindow();

return 0;
}

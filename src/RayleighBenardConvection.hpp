///////////////////////////////////////////////
//
//  RayleighBenardConvection.hpp specifies a class which is able to
//  perform perturbative calculations on a fixed environment,
//  describing a fluid dyamics evolution according to Navier-Stokes equations.  
//
/////////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "Eigen/Eigen"
#include "SuiteSparse_config.h"
#include "colamd.h"
#include "raylib.h"

#define T_CALDO 35
#define T_FREDDO 25

#define REFRESH_RATE 5
#define PIXEL 4
#define WINDOW_WIDTH ((PIXEL*nx) + 3*nx)
#define WINDOW_HEIGHT ((PIXEL*ny) + 3*ny)
#define NPROC 12
#define END_CICLE 5000


 class RayBenConvection {

   public:
     RayBenConvection();
     ~RayBenConvection();
     init();
     eval_next_frame();
     write_actual_frame();

   private:
     const std::vector<Color> jet {
(Color){  0,    0,  131,  255},
(Color){  0,    0,  135,  255},
(Color){  0,    0,  139,  255},
(Color){  0,    0,  143,  255},
(Color){  0,    0,  147,  255},
(Color){  0,    0,  151,  255},
(Color){  0,    0,  155,  255},
(Color){  0,    0,  159,  255},
(Color){  0,    0,  163,  255},
(Color){  0,    0,  167,  255},
(Color){  0,    0,  171,  255},
(Color){  0,    0,  175,  255},
(Color){  0,    0,  179,  255},
(Color){  0,    0,  183,  255},
(Color){  0,    0,  187,  255},
(Color){  0,    0,  191,  255},
(Color){  0,    0,  195,  255},
(Color){  0,    0,  199,  255},
(Color){  0,    0,  203,  255},
(Color){  0,    0,  207,  255},
(Color){  0,    0,  211,  255},
(Color){  0,    0,  215,  255},
(Color){  0,    0,  219,  255},
(Color){  0,    0,  223,  255},
(Color){  0,    0,  227,  255},
(Color){  0,    0,  231,  255},
(Color){  0,    0,  235,  255},
(Color){  0,    0,  239,  255},
(Color){  0,    0,  243,  255},
(Color){  0,    0,  247,  255},
(Color){  0,    0,  251,  255},
(Color){  0,    0,  255,  255},
(Color){  0,    4,  255,  255},
(Color){  0,    8,  255,  255},
(Color){  0,   12,  255,  255},
(Color){  0,   16,  255,  255},
(Color){  0,   20,  255,  255},
(Color){  0,   24,  255,  255},
(Color){  0,   28,  255,  255},
(Color){  0,   32,  255,  255},
(Color){  0,   36,  255,  255},
(Color){  0,   40,  255,  255},
(Color){  0,   44,  255,  255},
(Color){  0,   48,  255,  255},
(Color){  0,   52,  255,  255},
(Color){  0,   56,  255,  255},
(Color){  0,   60,  255,  255},
(Color){  0,   64,  255,  255},
(Color){  0,   68,  255,  255},
(Color){  0,   72,  255,  255},
(Color){  0,   76,  255,  255},
(Color){  0,   80,  255,  255},
(Color){  0,   84,  255,  255},
(Color){  0,   88,  255,  255},
(Color){  0,   92,  255,  255},
(Color){  0,   96,  255,  255},
(Color){  0,  100,  255,  255},
(Color){  0,  104,  255,  255},
(Color){  0,  108,  255,  255},
(Color){  0,  112,  255,  255},
(Color){  0,  116,  255,  255},
(Color){  0,  120,  255,  255},
(Color){  0,  124,  255,  255},
(Color){  0,  128,  255,  255},
(Color){  0,  131,  255,  255},
(Color){  0,  135,  255,  255},
(Color){  0,  139,  255,  255},
(Color){  0,  143,  255,  255},
(Color){  0,  147,  255,  255},
(Color){  0,  151,  255,  255},
(Color){  0,  155,  255,  255},
(Color){  0,  159,  255,  255},
(Color){  0,  163,  255,  255},
(Color){  0,  167,  255,  255},
(Color){  0,  171,  255,  255},
(Color){  0,  175,  255,  255},
(Color){  0,  179,  255,  255},
(Color){  0,  183,  255,  255},
(Color){  0,  187,  255,  255},
(Color){  0,  191,  255,  255},
(Color){  0,  195,  255,  255},
(Color){  0,  199,  255,  255},
(Color){  0,  203,  255,  255},
(Color){  0,  207,  255,  255},
(Color){  0,  211,  255,  255},
(Color){  0,  215,  255,  255},
(Color){  0,  219,  255,  255},
(Color){  0,  223,  255,  255},
(Color){  0,  227,  255,  255},
(Color){  0,  231,  255,  255},
(Color){  0,  235,  255,  255},
(Color){  0,  239,  255,  255},
(Color){  0,  243,  255,  255},
(Color){  0,  247,  255,  255},
(Color){  0,  251,  255,  255},
(Color){  0,  255,  255,  255},
(Color){  4,  255,  251,  255},
(Color){  8,  255,  247,  255},
(Color){ 12,  255,  243,  255},
(Color){ 16,  255,  239,  255},
(Color){ 20,  255,  235,  255},
(Color){ 24,  255,  231,  255},
(Color){ 28,  255,  227,  255},
(Color){ 32,  255,  223,  255},
(Color){ 36,  255,  219,  255},
(Color){ 40,  255,  215,  255},
(Color){ 44,  255,  211,  255},
(Color){ 48,  255,  207,  255},
(Color){ 52,  255,  203,  255},
(Color){ 56,  255,  199,  255},
(Color){ 60,  255,  195,  255},
(Color){ 64,  255,  191,  255},
(Color){ 68,  255,  187,  255},
(Color){ 72,  255,  183,  255},
(Color){ 76,  255,  179,  255},
(Color){ 80,  255,  175,  255},
(Color){ 84,  255,  171,  255},
(Color){ 88,  255,  167,  255},
(Color){ 92,  255,  163,  255},
(Color){ 96,  255,  159,  255},
(Color){100,  255,  155,  255},
(Color){104,  255,  151,  255},
(Color){108,  255,  147,  255},
(Color){112,  255,  143,  255},
(Color){116,  255,  139,  255},
(Color){120,  255,  135,  255},
(Color){124,  255,  131,  255},
(Color){128,  255,  128,  255},
(Color){131,  255,  124,  255},
(Color){135,  255,  120,  255},
(Color){139,  255,  116,  255},
(Color){143,  255,  112,  255},
(Color){147,  255,  108,  255},
(Color){151,  255,  104,  255},
(Color){155,  255,  100,  255},
(Color){159,  255,   96,  255},
(Color){163,  255,   92,  255},
(Color){167,  255,   88,  255},
(Color){171,  255,   84,  255},
(Color){175,  255,   80,  255},
(Color){179,  255,   76,  255},
(Color){183,  255,   72,  255},
(Color){187,  255,   68,  255},
(Color){191,  255,   64,  255},
(Color){195,  255,   60,  255},
(Color){199,  255,   56,  255},
(Color){203,  255,   52,  255},
(Color){207,  255,   48,  255},
(Color){211,  255,   44,  255},
(Color){215,  255,   40,  255},
(Color){219,  255,   36,  255},
(Color){223,  255,   32,  255},
(Color){227,  255,   28,  255},
(Color){231,  255,   24,  255},
(Color){235,  255,   20,  255},
(Color){239,  255,   16,  255},
(Color){243,  255,   12,  255},
(Color){247,  255,    8,  255},
(Color){251,  255,    4,  255},
(Color){255,  255,    0,  255},
(Color){255,  251,    0,  255},
(Color){255,  247,    0,  255},
(Color){255,  243,    0,  255},
(Color){255,  239,    0,  255},
(Color){255,  235,    0,  255},
(Color){255,  231,    0,  255},
(Color){255,  227,    0,  255},
(Color){255,  223,    0,  255},
(Color){255,  219,    0,  255},
(Color){255,  215,    0,  255},
(Color){255,  211,    0,  255},
(Color){255,  207,    0,  255},
(Color){255,  203,    0,  255},
(Color){255,  199,    0,  255},
(Color){255,  195,    0,  255},
(Color){255,  191,    0,  255},
(Color){255,  187,    0,  255},
(Color){255,  183,    0,  255},
(Color){255,  179,    0,  255},
(Color){255,  175,    0,  255},
(Color){255,  171,    0,  255},
(Color){255,  167,    0,  255},
(Color){255,  163,    0,  255},
(Color){255,  159,    0,  255},
(Color){255,  155,    0,  255},
(Color){255,  151,    0,  255},
(Color){255,  147,    0,  255},
(Color){255,  143,    0,  255},
(Color){255,  139,    0,  255},
(Color){255,  135,    0,  255},
(Color){255,  131,    0,  255},
(Color){255,  128,    0,  255},
(Color){255,  124,    0,  255},
(Color){255,  120,    0,  255},
(Color){255,  116,    0,  255},
(Color){255,  112,    0,  255},
(Color){255,  108,    0,  255},
(Color){255,  104,    0,  255},
(Color){255,  100,    0,  255},
(Color){255,   96,    0,  255},
(Color){255,   92,    0,  255},
(Color){255,   88,    0,  255},
(Color){255,   84,    0,  255},
(Color){255,   80,    0,  255},
(Color){255,   76,    0,  255},
(Color){255,   72,    0,  255},
(Color){255,   68,    0,  255},
(Color){255,   64,    0,  255},
(Color){255,   60,    0,  255},
(Color){255,   56,    0,  255},
(Color){255,   52,    0,  255},
(Color){255,   48,    0,  255},
(Color){255,   44,    0,  255},
(Color){255,   40,    0,  255},
(Color){255,   36,    0,  255},
(Color){255,   32,    0,  255},
(Color){255,   28,    0,  255},
(Color){255,   24,    0,  255},
(Color){255,   20,    0,  255},
(Color){255,   16,    0,  255},
(Color){255,   12,    0,  255},
(Color){255,    8,    0,  255},
(Color){255,    4,    0,  255},
(Color){255,    0,    0,  255},
(Color){251,    0,    0,  255},
(Color){247,    0,    0,  255},
(Color){243,    0,    0,  255},
(Color){239,    0,    0,  255},
(Color){235,    0,    0,  255},
(Color){231,    0,    0,  255},
(Color){227,    0,    0,  255},
(Color){223,    0,    0,  255},
(Color){219,    0,    0,  255},
(Color){215,    0,    0,  255},
(Color){211,    0,    0,  255},
(Color){207,    0,    0,  255},
(Color){203,    0,    0,  255},
(Color){199,    0,    0,  255},
(Color){195,    0,    0,  255},
(Color){191,    0,    0,  255},
(Color){187,    0,    0,  255},
(Color){183,    0,    0,  255},
(Color){179,    0,    0,  255},
(Color){175,    0,    0,  255},
(Color){171,    0,    0,  255},
(Color){167,    0,    0,  255},
(Color){163,    0,    0,  255},
(Color){159,    0,    0,  255},
(Color){155,    0,    0,  255},
(Color){151,    0,    0,  255},
(Color){147,    0,    0,  255},
(Color){143,    0,    0,  255},
(Color){139,    0,    0,  255},
(Color){135,    0,    0,  255},
(Color){131,    0,    0,  255},
(Color){128,    0,    0,  255}
};
 
     void printColor (Color );
     Color TtoC (double );
     Eigen::Matrix<double, nx, ny> T;
     void d_solve(Eigen::Matrix<double,-1,1> &,
             const Eigen::Matrix<int,1,-1> &,
             const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &,
             const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &);
     void Draw(const Eigen::Matrix<double, -1, -1> & ); 
};

  RayBenConvection::RayBenConvection() {}

  RayBenConvection::~RayBenConvection() {}

  void RayBenConvection::printColor (Color c) {
    ::printf("(%d, %d, %d, %d)\n", c.r, c.g, c.b, c.a);
  }

  Color RayBenConvection::TtoC (double T) {
    /*
  (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
  */
  int index = (T - T_FREDDO) * (jet.size() - 1) / ( T_CALDO - T_FREDDO );
  return jet[index];
  }

  void RayBenConvection::d_solve(Eigen::Matrix<double,-1,1> &mat,
             const Eigen::Matrix<int,1,-1> &perm,
             const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &L,
             const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &U) {
   mat(perm) = static_cast<Eigen::Matrix<double,-1,-1>>(U.solve(L.solve(mat(perm))));
  }


  void RayBenConvection:: Draw(const Eigen::Matrix<double, -1, -1> &T)
  {
  const unsigned int nx = T.rows();
  const unsigned int ny = T.cols();
  ::BeginDrawing();
    ::ClearBackground((Color){ 255, 255, 255, 255});
    for(unsigned int i = 0; i < nx; ++i) {
      for(unsigned int k = 0; k < ny; ++k) {
        ::DrawRectangle((WINDOW_WIDTH/2 - nx*PIXEL/2) + i*PIXEL, 
                        (WINDOW_HEIGHT/2 + ny*PIXEL/2) - k*PIXEL, 
                        PIXEL, PIXEL, TtoC(T(i, k)));
      }
    }
    
    int posY = 5;
    for(unsigned int i = 0; i < jet.size(); ++i) {
      ::DrawRectangle(5, posY, 5, 1, jet[i]);
      posY += 1;
    }
  ::EndDrawing();
  }


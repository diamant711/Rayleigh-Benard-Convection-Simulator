///////////////////////////////////////////////
//
//  RayleighBenardConvection.hpp specifies a class which is able to
//  perform perturbative calculations on a fixed environment,
//  describing fluid dyamics evolution according to Navier-Stokes equations.  
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

#define WINDOW_WIDTH ((PIXEL*m_nx) + 3*m_nx)
#define WINDOW_HEIGHT ((PIXEL*m_ny) + 3*m_ny)
#define NPROC 12
#define END_CICLE 5000


class RayBenConvection {

  public:
    RayBenConvection();
    ~RayBenConvection();
  void init();
  void eval_next_frame();
  void write_actual_frame();

  private:
  //variables
  static const unsigned int m_nx = 100;
  static const unsigned int m_ny = 70;
  const double m_tf = 1e3;
  double m_dt = 1e-2;
  const unsigned int m_nt = ((int) m_tf / m_dt) + 1;
 // dt = tf / nt;
  const double m_H = 2;
  const double m_L = 5;
  const double m_dx = m_L / (m_nx - 1);
  const double m_dy = m_H / (m_ny - 1);
  const double m_TN = T_FREDDO;
  const double m_TS = T_CALDO;
  Eigen::Matrix<double, m_nx+1, m_ny> m_u;
  Eigen::Matrix<double, m_nx, m_ny+1> m_v;
  Eigen::Matrix<double, m_nx, m_ny> m_p;
  Eigen::Matrix<double, m_nx, m_ny> m_S;
  Eigen::Matrix<double, m_nx, m_ny> m_uplot;
  Eigen::Matrix<double, m_nx, m_ny> m_vplot;
  double m_To;
  Eigen::Matrix<double, m_nx, m_ny> m_T;
  const double m_Re = 1e2;
  const double m_Pr = 7;
  const double m_Pe = m_Re * m_Pr;
  const double m_Ra = 1e2;
  const double m_Gr= m_Ra / m_Pr;
  Eigen::Matrix<double, m_nx, m_ny> m_Tstar;
  Eigen::Matrix<double, m_nx+1, m_ny> m_ustar;
  Eigen::Matrix<double, m_nx+1, m_ny> m_uhalf;
  Eigen::Matrix<double, m_nx+1, m_ny> m_uconv;
  Eigen::Matrix<double, m_nx, m_ny+1> m_vstar;
  Eigen::Matrix<double, m_nx, m_ny+1> m_vhalf;
  Eigen::Matrix<double, m_nx, m_ny+1> m_vconv;
  double m_TnE = 0;
  double m_TnW = 0;
  double m_UN = 0;
  double m_US = 0;
  double m_UE = 0;
  double m_UW = 0;
  double m_VN = 0;
  double m_VS = 0;
  double m_VE = 0;
  double m_VW = 0;
  Eigen::Matrix<double, m_nx-1, m_ny-2> m_bcu;
  Eigen::Matrix<double, m_nx-2, m_ny-1> m_bcv;
  Eigen::Matrix<double, -1, 1> m_e;
  Eigen::Matrix<double, -1, 1> m_i;
  Eigen::Matrix<double, -1, -1> m_Ax;
  Eigen::Matrix<double, -1, -1> m_Ay;
  Eigen::Matrix<double, -1, -1> m_A;
  Eigen::Matrix<double, -1, -1> m_Du;
  Eigen::Matrix<int, 1, -1> m_pu; 
  Eigen::Matrix<double, -1, -1> m_Duperm;
  std::vector<Eigen::Matrix<double, -1, -1>> m_LUu;
  Eigen::Matrix<double, -1, -1> m_Dv;
  Eigen::Matrix<int, 1, -1> m_pv;
  Eigen::Matrix<double, -1, -1> m_Dvperm;
  std::vector<Eigen::Matrix<double, -1, -1>> m_LUv;
  Eigen::Matrix<int, 1, -1> m_pp;
  Eigen::Matrix<double, -1, -1> m_Aperm;
  std::vector<Eigen::Matrix<double, -1, -1>> m_LUp;
  Eigen::Matrix<double, m_nx-2, m_ny-2> m_bcT;
  Eigen::Matrix<double, -1, -1> m_Tx;
  Eigen::Matrix<double, -1, -1> m_Ty;
  Eigen::Matrix<double, -1, -1> m_Tt;
  Eigen::Matrix<int, 1, -1> m_pt;
  Eigen::Matrix<double, -1, -1> m_Ttperm;
  std::vector<Eigen::Matrix<double, -1, -1>> m_LUt;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lu_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Uu_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lv_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Uv_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lp_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Up_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lt_solver;
  Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Ut_solver;
  const unsigned int m_ii = m_nx-2;
  const unsigned int m_jj = m_ny-2;


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

  //functions
    void printColor (Color );
    Color TtoC (double );
    void d_solve(Eigen::Matrix<double,-1,1> &,
            const Eigen::Matrix<int,1,-1> &,
            const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &,
            const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &);
    void Draw(const Eigen::Matrix<double, -1, -1> & );
    void ETA(int);

    template <typename Scalar> 
    Eigen::Matrix<Scalar,-1,-1> spdiags(const Eigen::Matrix<Scalar, -1, -1>& , 
                                    const Eigen::Matrix<int, -1, 1>& ,
                                    size_t , size_t );
    template <typename Scalar> 
    Eigen::Matrix<Scalar,-1,-1> kron(const Eigen::Matrix<Scalar, -1, -1>&,
                                 const Eigen::Matrix<Scalar, -1, -1>&); 
    template <typename Scalar>
    std::vector<Eigen::Triplet<Scalar>> SparseToTriplet(Eigen::SparseMatrix<Scalar> &);
    template <typename Scalar>
    Eigen::Matrix<int, 1, -1> my_symamd(Eigen::Matrix<Scalar, -1, -1> &); 
    void luP(Eigen::Matrix<double,-1,-1> &, std::vector<Eigen::Matrix<double,-1,-1>> &);

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


void RayBenConvection::Draw(const Eigen::Matrix<double, -1, -1> &T)
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

void RayBenConvection::ETA(int n_calls){
  static std::chrono::time_point<std::chrono::high_resolution_clock> old_time;
  float delta_t;

  ++n_calls;
  if(n_calls > 1) {
    delta_t = static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - old_time
        ).count()
    ) / 1000;
    if(delta_t > 0) {
      for(int i = 0; i < 80; ++i)
        std::putc(' ', stdout);
      std::cout << "\rETA: "<< delta_t * (END_CICLE - n_calls) << "s\t" 
                << 1.0/delta_t << "c/s\t("
                << n_calls << "/" << END_CICLE << ")\r";
      ::fflush(stdout);
    }
  }
  old_time = std::chrono::high_resolution_clock::now();
  return;
}

template <typename Scalar> 
Eigen::Matrix<Scalar,-1,-1> RayBenConvection::spdiags(const Eigen::Matrix<Scalar, -1, -1>& B, 
                                    const Eigen::Matrix<int, -1, 1>& d,
                                    size_t m, size_t n) {
  Eigen::SparseMatrix<Scalar> A(m,n);

  typedef Eigen::Triplet<Scalar> T;
  std::vector<T> triplets;
  triplets.reserve(std::min(m,n)*d.size());

  for (int k = 0; k < d.size(); k++) {
    int i_min = std::max(0, -d(k));
    int i_max = std::min(m - 1, n - d(k) - 1);
    int B_idx_start = m >= n ? d(k) : 0;

    for (int i = i_min; i <= i_max; i++) {
      triplets.push_back( T(i, i+d(k), B(B_idx_start + i, k)) );
    }
  }

  A.setFromTriplets(triplets.begin(), triplets.end());

  return Eigen::Matrix<Scalar, -1, -1>(A);
}

template <typename Scalar> 
Eigen::Matrix<Scalar,-1,-1> RayBenConvection::kron(const Eigen::Matrix<Scalar, -1, -1>& B, 
                                 const Eigen::Matrix<Scalar, -1, -1>& d) {
  Eigen::Matrix<Scalar, -1, -1> A;
  A.resize(B.rows()*d.rows(), B.cols()*d.cols());
  
  for (int i = 0; i < B.rows(); i++) 
    for (int j = 0; j < B.cols(); j++) 
      for (int h = 0; h < d.rows(); h++) 
        for (int k = 0; k < d.cols(); k++)
          A(i*d.rows() + h, j * d.cols() + k) = B(i, j) * d(h, k);  

  return Eigen::Matrix<Scalar, -1, -1>(A);
}

template <typename Scalar>
std::vector<Eigen::Triplet<Scalar>> RayBenConvection::SparseToTriplet(Eigen::SparseMatrix<Scalar> &A)
{
  std::vector<Eigen::Triplet<Scalar>> A_Triplet;
  for (int k = 0; k < A.outerSize(); ++k)
  {
	  for (Eigen::InnerIterator<Eigen::SparseMatrix<Scalar>> it(A, k); it; ++it)
	  {
		  A_Triplet.push_back(Eigen::Triplet<Scalar>(it.row(), it.col(), it.value()));
	  }
  }
  return A_Triplet;
}

template <typename Scalar>
Eigen::Matrix<int, 1, -1> RayBenConvection::my_symamd(Eigen::Matrix<Scalar, -1, -1> &mat) {
  if (mat.rows() != mat.cols()) {
    std::cerr << "symamd: Matrice non quadrata!" << std::endl;
    exit(1);
  }
  Eigen::SparseMatrix<Scalar> spmat(mat.sparseView()); 
  int A[spmat.nonZeros()];
  int p[mat.cols()+1];
  int perm[mat.cols()+1];
  int stats [COLAMD_STATS];
  int retcode;

  ::memset(A, 0, spmat.nonZeros());
  ::memset(p, 0, mat.cols()+1);
  ::memset(perm, 0, mat.cols()+1);
  ::memset(stats, 0, COLAMD_STATS);

  int A_i = 0, old_A_i = 0, p_i = 1;
  p[0] = 0;
  for(int i = 0; i < mat.cols(); ++i) {
    old_A_i = A_i;
    for(int k = 0; k < mat.rows(); ++k) {
      if(mat(k,i) != 0) {
        A[A_i] = k;
        A_i++;
      }
    }
    if(old_A_i < A_i){
      p[p_i] = A_i;
      p_i++;
    }
  }
  p[mat.cols()] = spmat.nonZeros();

  retcode = symamd(mat.cols(), A, p, perm, (double *) NULL, stats, &calloc, &free);

  if(retcode == 1){ 
    Eigen::Matrix<int, 1, -1> permMat;
    permMat.resize(1, mat.cols());
    for(int i = 0; i < mat.cols(); ++i)
      permMat(0, i) = perm[i];
    return permMat;
  } else if (retcode == 0) {
    std::cerr << "symamd: something wrong..." << std::endl;
    return Eigen::Matrix<int,1,1>(0);
  } else {
    std::cerr << "symamd: something really wrong happened!" << std::endl;
    return Eigen::Matrix<int,1,1>(0);
  }
}

void RayBenConvection::luP(Eigen::Matrix<double,-1,-1> &mat,
         std::vector<Eigen::Matrix<double,-1,-1>> &ret_vett){
  
  Eigen::SparseMatrix<double> spmat = mat.sparseView();
  Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> LU_dec(spmat);

  Eigen::Matrix<double,-1,-1> LU_mat(LU_dec.matrixLU());
  Eigen::Matrix<double,-1,-1> L_mat = LU_mat.triangularView<Eigen::UpLoType::UnitLower>();
  Eigen::Matrix<double,-1,-1> U_mat = LU_mat.triangularView<Eigen::UpLoType::Upper>();
  Eigen::Matrix<double,-1,-1> PL_mat = LU_dec.permutationP().transpose() * L_mat;

  ret_vett.clear();
  ret_vett.push_back(PL_mat); //[0]
  ret_vett.push_back(U_mat); //[1]
  
  return;
}

void RayBenConvection::init(){

  std::chrono::time_point<std::chrono::high_resolution_clock> init_lenght = 
    std::chrono::high_resolution_clock::now();
  Eigen::setNbThreads(NPROC);
  ::SetTraceLogLevel(LOG_WARNING);

  if(m_TS != m_TN)
    m_To = (m_TS < m_TN) ? m_TS : m_TN;
  else{
    std::cout << "Something stupid went wrong...";
    return;
  }

  //dt correction:
  m_dt = m_tf / m_nt;

  //initialising calculation maxtrices:
  m_u.setZero();
  m_v.setZero();
  m_p.setZero();
  m_S.setZero();
  m_uplot.setZero();
  m_vplot.setZero();
  m_T.setConstant(m_To);

  m_Tstar = m_T;
  m_ustar = m_u;
  m_uhalf = m_u;
  m_uconv = m_u;
  m_vstar = m_v;
  m_vhalf = m_v;
  m_vconv = m_v;

}

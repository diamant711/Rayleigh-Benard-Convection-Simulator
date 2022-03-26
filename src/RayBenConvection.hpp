
///////////////////////////////////////////////
//
//  RayleighBenardConvection.hpp specifies a class which is able to
//  perform perturbative calculations on a fixed environment,
//  describing fluid m_dyamics evolution according to Navier-Stokes equations.
//
/////////////////////////////////////////////////
/******************************************************************
 * TODO list:
 * X - Metodi privati vanno rinominati con il prefisso m_
 * 2 - Dopo aver scritto la documentazione sulla matematica: m_ii, m_jj -> nx-2, ny-2
 * X - write_current_frame() deve scrivere un file .h che conterra' una
 *     dichiarazione di una variabile globale dal nome poco rilevante che sara'†
 *     del tipo double[nx][ny][5000] ad ogni chiamata del metodo in oggetto
 *     verra' salvato T al valore attuale. (Scritto in semplice C99 -> no Eigen::
 *     e no std::)
 * 4 - Cercare di capire che analisi matematica vogliamo fare, propongo tre tipi
 *     di documentazione: a) un file LaTeX con le info molto leggere
 *     matematiche, una pagine di man per le info sull'utilizzo del software e
 *     una pagina stile doxigen con gli oggetti e i metodi
 * 5 - E' la corretta astrazione quella realizzata? Magari facciamo meglio il
 *     server web?
 * 6 - Quali parametri l'utente pu√≤ modificare?
 *
 ******************************************************************/


#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include "Eigen/Eigen"
#include "SuiteSparse_config.h"
#include "colamd.h"
#include "raylib.h"

#define REFRESH_RATE 5
#define PIXEL 4

#define WINDOW_WIDTH ((PIXEL*m_nx) + 3*m_nx)
#define WINDOW_HEIGHT ((PIXEL*m_ny) + 3*m_ny)
#define NPROC 12
#define OUTPUT_HEADER_FILE_NAME "temperature_matrix.h"

/*enum Warning_bytes {
   = 01000000000000000000000000000000,
  delta_temperature_over_10 = 00100000000000000000000000000000,
  TN_below_4 = 00010000000000000000000000000000,
}
*/

class RayBenConvection {

  public:
    RayBenConvection();
    ~RayBenConvection();
  int init(unsigned int, double, double, double, double, double );
  bool eval_next_frame();
  void write_current_data();

  private:
  //variables
  unsigned int m_END_CICLE;
  static const unsigned int m_nx = 100;
  static const unsigned int m_ny = 70;
  const double m_tf = 1e3;
  double m_dt = 1e-2;
  const unsigned int m_nt = ((int) m_tf / m_dt) + 1;
  const double m_H = 2;
  const double m_L = 5;
  const double m_dx = m_L / (m_nx - 1);
  const double m_dy = m_H / (m_ny - 1);
  double m_TN;  //lower temperature (cold)
  double m_TS;  //upper temperature (hot)
  Eigen::Matrix<double, m_nx+1, m_ny> m_u;
  Eigen::Matrix<double, m_nx, m_ny+1> m_v;
  Eigen::Matrix<double, m_nx, m_ny> m_p;
  Eigen::Matrix<double, m_nx, m_ny> m_S;
  double m_To;
  Eigen::Matrix<double, m_nx, m_ny> m_T;
  double m_Re;
  double m_Pr;
  double m_Pe;
  double m_Ra;
  double m_Gr;
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
  //initialisation variables
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
  static const unsigned int m_ii = m_nx-2;
  static const unsigned int m_jj = m_ny-2;
  //it: ex-for counter.
  unsigned int m_it;
  std::ofstream m_output_header_file;

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
  void m_apply_correction(void);
  void m_printColor (Color);
  Color m_TtoC (double, double, double);
  static void m_d_solve(Eigen::Matrix<double,-1,1> &,
          const Eigen::Matrix<int,1,-1> &,
          const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &,
          const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &);
  void m_Draw(double, double, const Eigen::Matrix<double, -1, -1> &);
  void m_ETA(int);

  template <typename Scalar> 
  Eigen::Matrix<Scalar,-1,-1> m_spdiags(const Eigen::Matrix<Scalar, -1, -1> &, 
                                  const Eigen::Matrix<int, -1, 1> &,
                                  size_t, size_t);
  template <typename Scalar> 
  Eigen::Matrix<Scalar,-1,-1> m_kron(const Eigen::Matrix<Scalar, -1, -1> &,
                               const Eigen::Matrix<Scalar, -1, -1> &);

  template <typename Scalar>
  std::vector<Eigen::Triplet<Scalar>> m_SparseToTriplet(Eigen::SparseMatrix<Scalar> &);
  template <typename Scalar>

  Eigen::Matrix<int, 1, -1> m_my_symamd(Eigen::Matrix<Scalar, -1, -1> &); 

  void m_luP(Eigen::Matrix<double,-1,-1> &, std::vector<Eigen::Matrix<double,-1,-1>> &);
  void m_write_current_frame();
};

RayBenConvection::RayBenConvection() {}

RayBenConvection::~RayBenConvection() {}

void RayBenConvection::m_apply_correction(void) {
  for(int i = 0; i < m_T.rows(); ++i) {
    for(int k = 0; k < m_T.cols(); ++k) {
      if((m_T(i, k) < m_TN) || (m_T(i, k) > m_TS)) {
        m_T(i, k) = m_TN;
      }
    }
  }
}

//void RayBenConvection::m_printColor (Color c) {
//  ::printf("(%d, %d, %d, %d)\n", c.r, c.g, c.b, c.a);
//}

Color RayBenConvection::m_TtoC (double cold_temp, double hot_temp,  double T) {
  /*
  (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
  */
  if ((T <= hot_temp) && (T >= cold_temp)) {
    int index = (T - cold_temp) * (jet.size() - 1) / ( hot_temp - cold_temp );
    return jet[index];
  } else {
    return (Color){0, 0, 0, 0};
  }
}


void RayBenConvection::m_d_solve(Eigen::Matrix<double,-1,1> &mat,
           const Eigen::Matrix<int,1,-1> &perm,
           const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &L,
           const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &U) {
  mat(perm) = static_cast<Eigen::Matrix<double,-1,-1>>(U.solve(L.solve(mat(perm))));
}


void RayBenConvection::m_Draw(double cold_temp, double hot_temp, const Eigen::Matrix<double, -1, -1> &T)
{
  const unsigned int nx = T.rows();
  const unsigned int ny = T.cols();
  ::BeginDrawing();
    ::ClearBackground((Color){ 255, 255, 255, 255});
    for(unsigned int i = 0; i < nx; ++i) {
      for(unsigned int k = 0; k < ny; ++k) {
        ::DrawRectangle((WINDOW_WIDTH/2 - nx*PIXEL/2) + i*PIXEL, 
                       (WINDOW_HEIGHT/2 + ny*PIXEL/2) - k*PIXEL, 
                       PIXEL, PIXEL, m_TtoC(cold_temp, hot_temp, T(i, k)));
      }
    }
  
    int posY = 5;
    for(unsigned int i = 0; i < jet.size(); ++i) {
      ::DrawRectangle(5, posY, 5, 1, jet[i]);
      posY += 1;
    }
  ::EndDrawing();
}

void RayBenConvection::m_ETA(int n_calls){
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
      std::cout << "\rETA: "<< delta_t * (m_END_CICLE - n_calls) << "s\t" 
                << 1.0/delta_t << "c/s\t("
                << n_calls << "/" << m_END_CICLE << ")\r";
      ::fflush(stdout);
    }
  }
  old_time = std::chrono::high_resolution_clock::now();
  return;
}

template <typename Scalar> 
Eigen::Matrix<Scalar,-1,-1> RayBenConvection::m_spdiags(const Eigen::Matrix<Scalar, -1, -1>& B, 
                                    const Eigen::Matrix<int, -1, 1>& d,
                                    size_t m, size_t n) {
  Eigen::SparseMatrix<Scalar> A(m,n);

  typedef Eigen::Triplet<Scalar> T;
  std::vector<T> triplets;
  triplets.reserve(std::min(m,n)*d.size());

  for (int k = 0; k < d.size(); k++) {
    int i_min = std::max(0, -d(k));
    int i_max = std::min(m - 1, n - d(k) - 1);
    int B_im_dx_start = m >= n ? d(k) : 0;

    for (int i = i_min; i <= i_max; i++) {
      triplets.push_back( T(i, i+d(k), B(B_im_dx_start + i, k)) );
    }
  }

  A.setFromTriplets(triplets.begin(), triplets.end());

  return Eigen::Matrix<Scalar, -1, -1>(A);
}

template <typename Scalar> 
Eigen::Matrix<Scalar,-1,-1> RayBenConvection::m_kron(const Eigen::Matrix<Scalar, -1, -1>& B, 
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
std::vector<Eigen::Triplet<Scalar>> RayBenConvection::m_SparseToTriplet(Eigen::SparseMatrix<Scalar> &A)
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
Eigen::Matrix<int, 1, -1> RayBenConvection::m_my_symamd(Eigen::Matrix<Scalar, -1, -1> &mat) {
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

void RayBenConvection::m_luP(Eigen::Matrix<double,-1,-1> &mat,
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

void RayBenConvection::m_write_current_frame (){
  m_output_header_file << "{";
    for (unsigned int i = 0; i < m_nx; i++) {
      m_output_header_file << "{";
        for (unsigned int j = 0; j < m_ny; j++)
          if (j < m_ny - 1)
            m_output_header_file << "\t" << m_T(i, j) << ", ";
          else m_output_header_file << "\t" << m_T(i, j); 
      m_output_header_file << "},\n ";
      }
  m_output_header_file << "}";
};

int RayBenConvection::init(unsigned int END_CICLE, double cold_temp, double hot_temp, 
                            double Ray_numb, double Pr_numb, double Re_numb ){

  std::chrono::time_point<std::chrono::high_resolution_clock> init_lenght = 
    std::chrono::high_resolution_clock::now();
  Eigen::setNbThreads(NPROC);
  ::SetTraceLogLevel(LOG_WARNING);

  int ret_value = 0;
  m_END_CICLE = END_CICLE;
  m_TN = cold_temp;
  m_TS = hot_temp;
  m_Ra = Ray_numb;
  m_Pr = Pr_numb;
  m_Re = Re_numb;
  m_Pe = m_Re * m_Pr;
  m_Gr= m_Ra / m_Pr;

  if (END_CICLE > 6000){
    std::cout<< "The number of steps requested "<<END_CICLE<<" is too high."<<std::endl;
    return -1;
    }

  if(m_TS != m_TN)
    m_To = (m_TS < m_TN) ? m_TS : m_TN;
  else{
    std::cout << "Please, remeber to provide two different temperatures.";
    return -2;
  }

  if (m_TN <= 0 || m_TN >= 100){
    std::cout << "Default parameters regard liquid water. Work in 0-100 range.";
    return -3;
  }

  if (Ray_numb < 50 || Ray_numb > 150 ){
    std::cout << "Working range for Rayleigh number: 50-150.";
    return -4;
  }

  if (Pr_numb < 5 || Pr_numb > 9){
    std::cout << "Working range for Prandtl number: 5-9.";
    return -5;
  }

  if (Re_numb < 70 ||  Re_numb > 130){
    std::cout << "Working range for Reynolds number: 70-130.";
    return -6;
  }

  if (m_TN < 4){
    std::cout << "Working with linear parameters. Liquid water "
                 "below of 4 Celsius does not follow linear behaviour." << std::endl;
    std::cout << "A simulation will be provided but the user is "
                 "advised to change working parameters." << std::endl;
    ret_value += 0b00010000000000000000000000000000;
  }

  if (m_TS - m_TN > 10){
    std::cout << "Warning: with this temperature range the linear" 
                " model validity is not guaranteed." << std::endl;
    std::cout << "The duration of the simulation will depend on the temperature range," 
                 "so nsteps requested cannot be assured." << std::endl; 
    ret_value += 0b00100000000000000000000000000000;
  }

  //m_dt correction:
  m_dt = m_tf / m_nt;

  //initialising calculation maxtrices:
  m_u.setZero();
  m_v.setZero();
  m_p.setZero();
  m_S.setZero();
  m_T.setConstant(m_To);

  m_Tstar = m_T;
  m_ustar = m_u;
  m_uhalf = m_u;
  m_uconv = m_u;
  m_vstar = m_v;
  m_vhalf = m_v;
  m_vconv = m_v;

  //IMPLICIT DIFFUSION:-
  //B.C vector(for u)
  m_bcu.setZero(),
  m_bcu.row(0).setConstant(2 * m_UW / std::pow(m_dx,2));
  m_bcu.row(m_bcu.rows()-1).setConstant(2 * m_UE / std::pow(m_dx,2));
  m_bcu.col(0).setConstant(m_US / std::pow(m_dy,2));
  m_bcu.col(m_bcu.cols()-1).setConstant(m_UN / std::pow(m_dy,2));

  //B.Cs at the corners:
  m_bcu(0, 0)                       = 2 * m_UW / std::pow(m_dx,2) + m_US / std::pow(m_dy,2);
  m_bcu(m_bcu.rows()-1, 0)            = 2 * m_UE / std::pow(m_dx,2) + m_US / std::pow(m_dy,2);
  m_bcu(0, (m_ny-2)-1)                = 2 * m_UW / std::pow(m_dx,2) + m_UN / std::pow(m_dy,2);
  m_bcu(m_bcu.rows()-1, m_bcu.cols()-1) = 2 * m_UE / std::pow(m_dx,2) + m_UN / std::pow(m_dy,2);
  m_bcu = m_dt * m_bcu / m_Re;

  //B.C vector(for v)
  m_bcv.setZero();
  m_bcv.row(0).setConstant(2 * m_VW / std::pow(m_dx,2));
  m_bcv.row(m_bcv.rows()-1).setConstant(2 * m_VE / std::pow(m_dx,2));
  m_bcv.col(0).setConstant(m_VS / std::pow(m_dy,2)); 
  m_bcv.col(m_bcv.cols()-1).setConstant(m_VN / std::pow(m_dy,2));
  
  //B.Cs at the corners:
  m_bcv(0, 0)                       = 2 * m_VW / std::pow(m_dx,2) + m_VS / std::pow(m_dy,2);
  m_bcv(m_bcv.rows()-1, 0)            = 2 * m_VE / std::pow(m_dx,2) + m_VS / std::pow(m_dy,2);
  m_bcv(0, m_bcv.cols()-1)            = 2 * m_VW / std::pow(m_dx,2) + m_VN / std::pow(m_dy,2);
  m_bcv(m_bcv.rows()-1, m_bcv.cols()-1) = 2 * m_VE / std::pow(m_dx,2) + m_VN / std::pow(m_dy,2);
  m_bcv = m_dt * m_bcv / m_Re;

  //Initialising central difference operator(for u)
  m_e.resize(m_nx-1, 1);
  m_i.resize(m_ny-2, 1);
  m_e.setOnes();
  m_i.setOnes();

  m_Ax = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_e * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1), 
    m_nx-1, m_nx-1
    );

  m_Ay = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_i * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1), 
    m_ny-2, m_ny-2
    );
  
  m_Ax(0, 0) = -3;
  m_Ax(m_Ax.rows()-1, m_Ax.rows()-1) = -3; 
   
  m_A = m_kron<double>(
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ay/std::pow(m_dy,2)), 
        Eigen::Matrix<double, m_nx-1, m_nx-1>::Identity()
        )
        + m_kron<double>(
        Eigen::Matrix<double, m_ny-2, m_ny-2>::Identity(),
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ax/std::pow(m_dx,2))
        );
  
  m_Du = Eigen::Matrix<double, (m_nx-1)*(m_ny-2), (m_nx-1)*(m_ny-2)>::Identity() - m_dt * m_A/m_Re;
  m_pu = m_my_symamd(m_Du);
  
  m_Duperm = m_Du(m_pu,m_pu);
  
  m_luP(m_Duperm, m_LUu);
  
  //Central difference operator(for v)
  m_e.resize(m_nx-2, 1);
  m_e.setOnes();
  m_i.resize(m_ny-1, 1);
  m_i.setOnes();
  
  m_Ax = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_e * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1), 
    m_nx-2, m_nx-2
    );
  
  m_Ay = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_i * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1), 
    m_ny-1, m_ny-1
    ); 
  
  m_Ay(0, 0) = -3;
  m_Ay(m_Ay.rows()-1, m_Ay.cols()-1) = -3; 
   
  m_A = m_kron<double>(
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ay/std::pow(m_dy,2)), 
        Eigen::Matrix<double, m_nx-2, m_nx-2>::Identity()
      )
    + m_kron<double>(
        Eigen::Matrix<double, m_ny-1, m_ny-1>::Identity(),
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ax/std::pow(m_dx,2))
      );

  m_Dv = Eigen::Matrix<double, (m_nx-2)*(m_ny-1), (m_nx-2)*(m_ny-1)>::Identity() - m_dt * m_A/m_Re;
  m_pv = m_my_symamd(m_Dv);

  m_Dvperm = m_Dv(m_pv,m_pv);

  m_luP(m_Dvperm, m_LUv);

  //
  //Calculating the coefficient matrix for the PPE
  m_e.resize(m_nx-2, 1);
  m_e.setOnes();
  m_i.resize(m_ny-2, 1);
  m_i.setOnes();

  m_Ax = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_e * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1), 
    m_nx-2, m_nx-2
    ); 

  m_Ay = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_i * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1), 
    m_ny-2, m_ny-2
    ); 
  
  m_Ax(0, 0) = -1;
  m_Ay(0, 0) = -1;
  m_Ax(m_Ax.rows()-1, m_Ax.cols()-1) = -1;
  m_Ay(m_Ay.rows()-1, m_Ay.cols()-1) = -1;


  m_A = m_kron<double>(
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ay/std::pow(m_dy,2)), 
        Eigen::Matrix<double, m_nx-2, m_nx-2>::Identity()
      )
    + m_kron<double>(
        Eigen::Matrix<double, m_ny-2, m_ny-2>::Identity(),
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ax/std::pow(m_dx,2))
      );

  m_pp = m_my_symamd(m_A);

  m_Aperm = m_A(m_pp,m_pp);

  m_luP(m_Aperm, m_LUp);

  //%%
  //%B.C vector(for T)
  m_bcT.setZero();
  m_bcT.row(0).setConstant(- m_TnW / m_dx); 
  m_bcT.row(m_bcT.rows()-1).setConstant(m_TnE / m_dx);
  m_bcT.col(0).setConstant(m_TS / std::pow(m_dy,2)); 
  m_bcT.col(m_bcT.cols()-1).setConstant(m_TN / std::pow(m_dy,2));

  //%B.Cs at the corners:
  m_bcT(0, 0)                       = - m_TnW / m_dx + m_TS / std::pow(m_dy,2);
  m_bcT(m_bcT.rows()-1, 0)            = m_TnE / m_dx + m_TS / std::pow(m_dy,2);
  m_bcT(0, m_bcT.cols()-1)            = - m_TnW / m_dx + m_TN / std::pow(m_dy,2);
  m_bcT(m_bcT.rows()-1, m_bcT.cols()-1) = m_TnE / m_dx + m_TN / std::pow(m_dy,2);
  m_bcT = m_dt * m_bcT / m_Pe;

  //Central difference operator(for T)
  m_e.resize(m_nx-2, 1);
  m_e.setOnes();
  m_i.resize(m_ny-2, 1);
  m_i.setOnes();

  m_Tx = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_e * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1),
    m_nx-2, m_nx-2
    );

  m_Ty = m_spdiags(
    static_cast<Eigen::Matrix<double, -1, -1>>(m_i * Eigen::Matrix<double, 1, 3>(1, -2, 1)),
    Eigen::Matrix<int, 1, 3> (-1, 0, 1),
    m_ny-2, m_ny-2
    );

  m_Tx(0, 0) = -1;
  m_Tx(m_Tx.rows()-1, m_Tx.cols()-1) = -1; 

  m_Tt = m_kron<double>(
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Ty/std::pow(m_dy,2)),
  Eigen::Matrix<double, m_nx-2, m_nx-2>::Identity()
      )
      + m_kron<double>(
        Eigen::Matrix<double, m_ny-2, m_ny-2>::Identity(),
        static_cast<Eigen::Matrix<double, -1, -1>>(m_Tx/std::pow(m_dx,2))
      );

  m_Tt = Eigen::Matrix<double, (m_nx-2)*(m_ny-2), (m_nx-2)*(m_ny-2)>::Identity() - m_dt * m_Tt/m_Pe;

  m_pt = m_my_symamd(m_Tt);

  m_Ttperm = m_Tt(m_pt,m_pt);

  m_luP(m_Ttperm, m_LUt);

  //%%
  //%Boundary conditions
  m_T.row(0) = m_T.row(1) - Eigen::Matrix<double, 1, m_ny>::Constant(m_TnW * m_dx);
  m_T.row(m_T.rows()-1) = m_T.row(m_T.rows()-2) + Eigen::Matrix<double, 1, m_ny>::Constant(m_TnE * m_dx);
  m_T.col(0).setConstant(m_TS);
  m_T.col(m_T.cols()-1).setConstant(m_TN);

  m_u.row(0) = Eigen::Matrix<double, 1, m_ny>::Constant(2 * m_UW) - m_u.row(1);
  m_u.row(m_u.rows()-1) = Eigen::Matrix<double, 1, m_ny>::Constant(2 * m_UE) - m_u.row(m_u.rows()-2);
  m_u.col(0).setConstant(m_US);
  m_u.col(m_u.cols()-1).setConstant(m_UN);

  m_v.row(0).setConstant(m_VW);
  m_v.row(m_v.rows()-1).setConstant(m_VE);
  m_v.col(0) = Eigen::Matrix<double, m_nx, 1>::Constant(2 * m_VS) - m_v.col(1);
  m_v.col(m_v.cols()-1) = Eigen::Matrix<double, m_nx, 1>::Constant(2 * m_VN) - m_v.col(m_v.cols()-2);

  //FIXME: questo commento annuncia l'inizio del for?  
  //%%
  //%Evaluating temperature and velocity field at each time step

  //Easying solver calculations: eleoicca stuff
  m_Lu_solver.compute(m_LUu[0]);

  m_Uu_solver.compute(m_LUu[1]);

  m_Lv_solver.compute(m_LUv[0]);

  m_Uv_solver.compute(m_LUv[1]);

  m_Lp_solver.compute(m_LUp[0]);

  m_Up_solver.compute(m_LUp[1]);

  m_Lt_solver.compute(m_LUt[0]);

  m_Ut_solver.compute(m_LUt[1]);

  std::cout << "Time spent in init() function: "
  << static_cast<float>(
       std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - init_lenght
       ).count()
     ) / 1000
  << "s" << std::endl;

  m_it=0;


//  ::InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rayleigh-Benard Convection");

return ret_value;

}

bool RayBenConvection::eval_next_frame(){

  //const unsigned int m_ii = m_nx-2;
  //const unsigned int m_jj = m_ny-2;

  if (m_it < m_END_CICLE ) {

   m_ETA(m_it);

    m_apply_correction();


//    if(m_it % REFRESH_RATE == 0) {
//      m_Draw(m_TN, m_TS, m_T);
//    }

    Eigen::Matrix<double, -1, -1> Tn(m_T);
    m_Tstar.block(1,1,m_ii,m_jj) = (Tn.block(1,1,m_ii,m_jj).array() - (m_dt / m_dx / 2)
      * (m_u.block(2,1,m_ii,m_jj).array() * (Tn.block(1,1,m_ii,m_jj) + Tn.block(2,1,m_ii,m_jj)).array() 
      - m_u.block(1,1,m_ii,m_jj).array() * (Tn.block(1,1,m_ii,m_jj) + Tn.block(0,1,m_ii,m_jj)).array()) 
      - (m_dt / m_dy / 2)
      * (m_v.block(1,2,m_ii,m_jj).array() * (Tn.block(1,1,m_ii,m_jj) + Tn.block(1,2,m_ii,m_jj)).array()
      - m_v.block(1,1,m_ii,m_jj).array() * (Tn.block(1,1,m_ii,m_jj) + Tn.block(1,0,m_ii,m_jj)).array())).matrix();

    Eigen::Matrix<double, -1, 1> t;
    t.resize(m_bcT.rows()*m_bcT.cols(), 1);
    t.col(0) = (m_Tstar.block(1,1,m_Tstar.rows()-2,m_Tstar.cols()-2) + m_bcT).reshaped(m_bcT.rows()*m_bcT.cols(), 1);

    t(m_pt) = static_cast<Eigen::Matrix<double, -1, -1>>(m_Ut_solver.solve(m_Lt_solver.solve(t(m_pt))));

    Eigen::Matrix<double, -1, -1> new_t = t.reshaped(m_nx-2, m_ny-2);

    m_T.block(1,1,m_T.rows()-2,m_T.cols()-2) = new_t;

    m_T.row(0) = m_T.row(1) - Eigen::Matrix<double, 1, m_ny>::Constant(m_TnW * m_dx);
    m_T.row(m_T.rows()-1) = m_T.row(m_T.rows()-2) + Eigen::Matrix<double, 1, m_ny>::Constant(m_TnE * m_dx);

    m_T.col(0).setConstant(m_TS);
    m_T.col(m_T.cols()-1).setConstant(m_TN);

    Eigen::Matrix<double, -1, -1> un = m_u;
    Eigen::Matrix<double, -1, -1> vn = m_v;

    //%Predictor step(Lax-Friedrich)
    m_uhalf.block(2,1,m_ii,m_jj) = (0.5 * (un.block(3,1,m_ii,m_jj) + un.block(1,1,m_ii,m_jj)).array() - (m_dt / m_dx / 8)
                    * (un.block(3,1,m_ii,m_jj) + un.block(2,1,m_ii,m_jj)).array().square() - (un.block(2,1,m_ii,m_jj)
                    + un.block(1,1,m_ii,m_jj)).array().square() - (m_dt / m_dy / 8) * ((un.block(2,1,m_ii,m_jj)
                    + un.block(2,2,m_ii,m_jj)).array() * (vn.block(1,2,m_ii,m_jj) + vn.block(2,2,m_ii,m_jj)).array()
                    - (un.block(2,1,m_ii,m_jj) + un.block(2,0,m_ii,m_jj)).array() * (vn.block(1,1,m_ii,m_jj)
                    + vn.block(2,1,m_ii,m_jj)).array())).matrix();
    m_vhalf.block(1,2,m_ii,m_jj) = (0.5 * (vn.block(1,3,m_ii,m_jj) + vn.block(1,1,m_ii,m_jj)).array() - (m_dt / m_dx / 8)
                    * ((un.block(2,1,m_ii,m_jj) + un.block(2,2,m_ii,m_jj)).array() * (vn.block(1,2,m_ii,m_jj) 
                    + vn.block(2,2,m_ii,m_jj)).array() - (un.block(1,1,m_ii,m_jj) + un.block(1,0,m_ii,m_jj)).array()
                    * (vn.block(1,2,m_ii,m_jj)
                    + vn.block(0,2,m_ii,m_jj)).array()) - (m_dt / m_dy / 8) * ((vn.block(1,3,m_ii,m_jj) 
                    + vn.block(1,2,m_ii,m_jj)).array().square() - (vn.block(1,2,m_ii,m_jj)
                    + vn.block(1,1,m_ii,m_jj)).array().square())).matrix();
    //%Corrector step(Leapfrog)
    m_uconv.block(2,1,m_ii,m_jj) = (un.block(2,1,m_ii,m_jj).array() - (m_dt / (4 * m_dx)) * ((m_uhalf.block(3,1,m_ii,m_jj) 
                    + m_uhalf.block(2,1,m_ii,m_jj)).array().square() - (m_uhalf.block(2,1,m_ii,m_jj)
                    + m_uhalf.block(1,1,m_ii,m_jj)).array().square()) - (m_dt / (4 * m_dy)) * ((m_uhalf.block(2,1,m_ii,m_jj) 
                    + m_uhalf.block(2,2,m_ii,m_jj)).array() * (m_vhalf.block(1,2,m_ii,m_jj) + m_vhalf.block(2,2,m_ii,m_jj)).array()
                    - (m_uhalf.block(2,1,m_ii,m_jj) + m_uhalf.block(2,0,m_ii,m_jj)).array() * (m_vhalf.block(1,1,m_ii,m_jj) 
                    + m_vhalf.block(2,1,m_ii,m_jj)).array())).matrix();
    m_vconv.block(1,2,m_ii,m_jj) = (vn.block(1,2,m_ii,m_jj).array() - (m_dt / (4 * m_dx)) * ((m_uhalf.block(2,1,m_ii,m_jj) 
                    + m_uhalf.block(2,2,m_ii,m_jj)).array() * (m_vhalf.block(1,2,m_ii,m_jj) + m_vhalf.block(2,2,m_ii,m_jj)).array()
                    - (m_uhalf.block(1,1,m_ii,m_jj) + m_uhalf.block(1,0,m_ii,m_jj)).array() * (m_vhalf.block(1,2,m_ii,m_jj)
                    + m_vhalf.block(0,2,m_ii,m_jj)).array()) - (m_dt / (4 * m_dy)) * ((m_vhalf.block(1,3,m_ii,m_jj)
                    + m_vhalf.block(1,2,m_ii,m_jj)).array().square() 
                    - (m_vhalf.block(1,2,m_ii,m_jj) + m_vhalf.block(1,1,m_ii,m_jj)).array().square())).matrix();
    //%Buoyancy term (Boussinesq aproximation):
    m_vconv.block(1,2,m_ii,m_jj) = m_vconv.block(1,2,m_ii,m_jj) + (m_Gr / std::pow(m_Re, 2)) * (0.5 * (m_T.block(1,1,m_ii,m_jj)
                    + m_T.block(1,2,m_ii,m_jj)) - Eigen::Matrix<double, m_ii, m_jj>::Constant(m_To));

    //%(2) Implicit central difference (spatial)
    Eigen::Matrix<double, -1, 1> U;
    U.resize(m_bcu.rows()*m_bcu.cols(), 1);

    U.col(0) = (m_uconv.block(1,1,m_uconv.rows()-2,m_uconv.cols()-2) + m_bcu).reshaped(m_bcu.rows()*m_bcu.cols(), 1);

    Eigen::Matrix<double, -1, 1> V;
    V.resize(m_bcv.rows()*m_bcv.cols(), 1);

    V.col(0) = (m_vconv.block(1,1,m_vconv.rows()-2,m_vconv.cols()-2) + m_bcv).reshaped(m_bcv.rows()*m_bcv.cols(), 1);

    std::thread t1(m_d_solve, std::ref(U), std::ref(m_pu), std::ref(m_Lu_solver), std::ref(m_Uu_solver));
    std::thread t2(m_d_solve, std::ref(V), std::ref(m_pv), std::ref(m_Lv_solver), std::ref(m_Uv_solver));

    t1.join();
    t2.join();

    Eigen::Matrix<double, -1, -1> new_U = U.reshaped(m_nx-1, m_ny-2);

    m_ustar.block(1,1,m_ustar.rows()-2,m_ustar.cols()-2) = new_U;

    Eigen::Matrix<double, -1, -1> new_V = V.reshaped(m_nx-2, m_ny-1);

    m_vstar.block(1,1,m_vstar.rows()-2,m_vstar.cols()-2) = new_V;

    //%Pressure Poisson equation(elliptic):
    m_S.block(1,1,m_ii,m_jj) = (m_ustar.block(2,1,m_ii,m_jj) - m_ustar.block(1,1,m_ii,m_jj)) / m_dx
                            + (m_vstar.block(1,2,m_ii,m_jj) - m_vstar.block(1,1,m_ii,m_jj)) / m_dy;

    Eigen::Matrix<double, -1, 1> s;
    s.resize((m_S.rows()-2)*(m_S.cols()-2), 1);

    s.col(0) = m_S.block(1,1,m_S.rows()-2,m_S.cols()-2).reshaped((m_S.rows()-2)*(m_S.cols()-2), 1);

    s(m_pp) = static_cast<Eigen::Matrix<double, -1, -1>>(m_Up_solver.solve(m_Lp_solver.solve(s(m_pp))));

    Eigen::Matrix<double, -1, -1> new_s = s.reshaped(m_nx-2, m_ny-2);

    m_p.block(1,1,new_s.rows(),new_s.cols()) = new_s;

    m_p.row(0) = m_p.row(1);
    m_p.row(m_p.rows()-1) = m_p.row(m_p.rows()-2);

    m_p.col(0) = m_p.col(1);
    m_p.col(m_p.cols()-1) = m_p.col(m_p.cols()-2);

    m_u.block(2,1,m_ii,m_jj) = m_ustar.block(2,1,m_ii,m_jj) - (m_p.block(2,1,m_ii,m_jj) - m_p.block(1,1,m_ii,m_jj)) / m_dx;
    m_v.block(1,2,m_ii,m_jj) = m_vstar.block(1,2,m_ii,m_jj) - (m_p.block(1,2,m_ii,m_jj) - m_p.block(1,1,m_ii,m_jj)) / m_dy;

    m_u.row(0) = Eigen::Matrix<double, 1, m_ny>::Constant(2*m_UW) - m_u.row(1);
    m_u.row(m_u.rows()-1) = Eigen::Matrix<double, 1, m_ny>::Constant(2*m_UE) - m_u.row(m_u.rows()-2);

    m_u.col(0).setConstant(m_US);
    m_u.col(m_u.cols()-1).setConstant(m_UN);

    m_v.row(0).setConstant(m_VW);
    m_v.row(m_v.rows()-1).setConstant(m_VE);

    m_v.col(0) = Eigen::Matrix<double, m_nx, 1>::Constant(2*m_VS) - m_v.col(1);
    m_v.col(m_v.cols()-1) = Eigen::Matrix<double, m_nx, 1>::Constant(2*m_VN) - m_v.col(m_v.cols()-2);

  }


  if (m_it == m_END_CICLE) {
    std::cout << "FINE" << std::endl;
//    ::CloseWindow();
    return true;
  }


  ++m_it;

  return false;
}

void RayBenConvection::write_current_data(){
  if (m_it == 0){
    m_output_header_file.open(OUTPUT_HEADER_FILE_NAME, std::ios_base::out);
   std::cout << "File aperto"; 
    if (! m_output_header_file.is_open()){
      std::cerr <<"Failed opening output_header_file. Exiting..." <<std::endl;
      return;
      }
  m_output_header_file << "const unsigned int nx = " << m_nx << ";\n" 
                          "const unsigned int ny = " << m_ny << ";\n"
                          "const unsigned int n_steps = " << m_END_CICLE << ";\n" 
                          "const unsigned double cold_temp = " << m_TN << ";\n"
                          "const unsigned double hot_temp = " << m_TS << ";\n"
                          "const double T [nx][ny][n_steps] = {\n";
  m_write_current_frame();
  }
  else if (m_it == m_END_CICLE){
    m_output_header_file << "}; ";
    m_output_header_file.close();
   std::cout << "File chiuso";
   }
  else {
    m_output_header_file <<",\n";
    m_write_current_frame();
  }
}

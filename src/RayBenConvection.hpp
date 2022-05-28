/******************************************************************
 * TODO list:
 * X - Metodi privati vanno rinominati con il prefisso m_
       * 2 - Dopo aver scritto la documentazione sulla matematica: m_ii, m_jj -> nx-2, ny-2
 * X - write_current_frame() deve scrivere un file .h che conterra' una
 *     dichiarazione di una variabile globale dal nome poco rilevante che sara' 
 *     del tipo double[nx][ny][5000] ad ogni chiamata del metodo in oggetto
 *     verra' salvato T al valore attuale. (Scritto in semplice C99 -> no Eigen::
 *     e no std::)
 * 4 - Cercare di capire che analisi matematica vogliamo fare, propongo tre tipi
 *     di documentazione: a) un file LaTeX con le info molto leggere
 *     matematiche, una pagine di man per le info sull'utilizzo del software e
 *     una pagina stile doxigen con gli oggetti e i metodi
 * 5 - E' la corretta astrazione quella realizzata? Magari facciamo meglio il
 *     server web?
 * X - Quali parametri l'utente puÃ² modificare?
 *
 *LEGENDA ERRORI:
 * return -1  END_CICLE > 6000
 * return -2  m_TS=m_TN
 * return -3  m_TN <= 0 oppure m_TN >= m_TS
 * return -4  Ray_numb < 50 || > 150
 * return -5  Pr_numb < 5 || > 9
 * return -6  Re_numb < 70 || > 130
 * return -7  m_TS >= 100

 ******************************************************************/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>

#include "Eigen/Eigen"
#include "SuiteSparse_config.h"
#include "colamd.h"

#define NPROC 12
#define OUTPUT_HEADER_FILE_NAME "inc/temperature_matrix.h"

/*enum Warning_bytes {
  delta_temperature_over_10 = 00100000000000000000000000000000,
  TN_below_4 = 00010000000000000000000000000000,
}
*/

//! RayBenConvection is the core of the numerical simulation.
/*!
  This class performs perturbative calculations on a fixed environment,
  describing fluid dynamics evolution according to Navier-Stokes equations.
*/

class RayBenConvection {

  public:

    //! This struct represents the advancement of the simulation.
    typedef struct {
      /*! This parameter represents if the simulation has ended without errors.  */
      bool ended;
      /*! This parameter represents the numer of times the calculation will be iterated. */
      int total;
      /*! This parameter represents the estimated time of arrival, i.e. the time left before the end of the calculation.  */
      int eta;
      /*! This parameter represents the number of iterations performed per second. */
      float velocity;
      int step;
      } simulation_state_t;

    //! Class constructor.
    RayBenConvection(void);
    //! Class destructor.
    ~RayBenConvection(void);

    int init(unsigned int, double, double, double, double, double);

    simulation_state_t eval_next_frame(void);

    void write_current_data(void);

  private:
    //variables
    //! total number of steps performed.
    unsigned int m_END_CICLE;
    //! size of x axis
    static const unsigned int m_nx = 100;
    //! size of y axis, along which we apply the gravitational force.
    static const unsigned int m_ny = 70;
    //! implementation detail.
    const double m_tf = 1e3;
    //! interval of time used for perturbative calculation.
    double m_dt = 1e-2;
    //! implementation detail.
    const unsigned int m_nt = ((int) m_tf / m_dt) + 1;
    //! height of the box of the simulation.
    /*!
     The ratio of m_H and m_L is an important physical parameter for the simulation,
     which influences all other parameters, such as m_Pr, m_Re, m_Ra. 
     Since it is not possible to forsee how changing this ratio will modify the other parameters,
     the user must not have access to m_H and m_L.
    */
    const double m_H = 2;
    //! lenght of the box of the simulation.
    /*!
    \sa m_H
    */
    const double m_L = 5;
    //! interval of x coordinate used for perturbative calculation.
    const double m_dx = m_L / (m_nx - 1);
    //! interval of y coordinate used for perturbative calculation.
    const double m_dy = m_H / (m_ny - 1);
    //! lower temperature
    double m_TN;
    //! higher temperature
    double m_TS;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx+1, m_ny> m_u;
    //! Eigen matrix used for perturbative calculation on the fluid's velocity.
    Eigen::Matrix<double, m_nx, m_ny+1> m_v;
    //! Eigen matrix used for perturbative calculation on the fluid's pressure.
    Eigen::Matrix<double, m_nx, m_ny> m_p;
    //! Eigen matrix used for perturbative calculation on the fluid's pressure.
    Eigen::Matrix<double, m_nx, m_ny> m_S;
    //! implementation detail.
    double m_To;
    //! Eigen maxtrix that represents the temperature of every point per frame. 
    Eigen::Matrix<double, m_nx, m_ny> m_T;
    //! Reynolds numer.
    double m_Re;
    //! Prandtl number.
    double m_Pr;
    //!
    double m_Pe;
    //! Rayleigh number.
    double m_Ra;
    //!
    double m_Gr;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx, m_ny> m_Tstar;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx+1, m_ny> m_ustar;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx+1, m_ny> m_uhalf;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx+1, m_ny> m_uconv;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx, m_ny+1> m_vstar;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx, m_ny+1> m_vhalf;
    //! Eigen matrix used for perturbative calculation.
    Eigen::Matrix<double, m_nx, m_ny+1> m_vconv;
    //! implementation detail.
    double m_TnE = 0;
    //! implementation detail.
    double m_TnW = 0;
    //! implementation detail.
    double m_UN = 0;
    //! implementation detail.
    double m_US = 0;
    //! implementation detail.
    double m_UE = 0;
    //! implementation detail.
    double m_UW = 0;
    //! implementation detail.
    double m_VN = 0;
    //! implementation detail.
    double m_VS = 0;
    //! implementation detail.
    double m_VE = 0;
    //! implementation detail.
    double m_VW = 0;
    //! Eigen matrix used for boundary conditions.
    Eigen::Matrix<double, m_nx-1, m_ny-2> m_bcu;
    //! Eigen matrix used for boundary conditions.
    Eigen::Matrix<double, m_nx-2, m_ny-1> m_bcv;
    //! Eigen matrix used for calculations support.
    Eigen::Matrix<double, -1, 1> m_e;
    //! Eigen matrix used for calculations support.
    Eigen::Matrix<double, -1, 1> m_i;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Ax;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Ay;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_A;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Du;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<int, 1, -1> m_pu;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Duperm;
    //! std vector of Eigen matrices.
    std::vector<Eigen::Matrix<double, -1, -1>> m_LUu;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Dv;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<int, 1, -1> m_pv;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Dvperm;
    // std vector of Eigen matrices.
    std::vector<Eigen::Matrix<double, -1, -1>> m_LUv;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<int, 1, -1> m_pp;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Aperm;
    //! std vector of Eigen matrices.
    std::vector<Eigen::Matrix<double, -1, -1>> m_LUp;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, m_nx-2, m_ny-2> m_bcT;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Tx;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Ty;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Tt;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<int, 1, -1> m_pt;
    //! Eigen matrix used as calculation support.
    Eigen::Matrix<double, -1, -1> m_Ttperm;
    //! std vector of Eigen matrices.
    std::vector<Eigen::Matrix<double, -1, -1>> m_LUt;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lu_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Uu_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lv_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Uv_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lp_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Up_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Lt_solver;
    //! Eigen PartialPivLu matrix, used for perfoming partial pivot.
    Eigen::PartialPivLU<Eigen::Matrix<double, -1, -1>> m_Ut_solver;
    //! implementation detail.
    static const unsigned int m_ii = m_nx-2;
    //! implementation detail.
    static const unsigned int m_jj = m_ny-2;
    //! current step of the simulation.
    unsigned int m_it;
    //! output .h file, in which all temperature datas of the simulation are saved. 
    std::ofstream m_output_header_file;
    /*!
     \sa simulation_state_t
    */
    simulation_state_t simulation_state;

    //functions
    void m_apply_correction(void);
    static void m_d_solve(Eigen::Matrix<double,-1,1> &,
            const Eigen::Matrix<int,1,-1> &,
            const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &,
            const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &);
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

//! This function applies small corrections on m_T matrix.
/*!
 During the perturbative calculations, small errors due to the truncation of the decimals may occur,
 resulting in temperatures lower than m_TN.
 Since these values do not hold any physical value nor represent a calculation error,
 this function replaces them with m_TN.
 \sa m_TN
*/
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

//Color RayBenConvection::m_TtoC (double cold_temp, double hot_temp,  double T) {
  /*
  (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
  */
//  if ((T <= hot_temp) && (T >= cold_temp)) {
//    int index = (T - cold_temp) * (jet.size() - 1) / ( hot_temp - cold_temp );
//    return jet[index];
//  } else {
//    return (Color){0, 0, 0, 0};
//  }
//}

//! This function returns a permutated matrix, following Eigen  Partial Pivoting operation.
/*!
 \param mat Eigen matrix to be permutated.
 \param perm Eigen vector of permutation.
 \param L Eigen PartialPivLu matrix.
 \param U Eigen PartialPivLu matrix.
*/
void RayBenConvection::m_d_solve(Eigen::Matrix<double,-1,1> &mat,
           const Eigen::Matrix<int,1,-1> &perm,
           const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &L,
           const Eigen::PartialPivLU<Eigen::Matrix<double,-1,-1>> &U) {
  mat(perm) = static_cast<Eigen::Matrix<double,-1,-1>>(U.solve(L.solve(mat(perm))));
}


//void RayBenConvection::m_Draw(double cold_temp, double hot_temp, const Eigen::Matrix<double, -1, -1> &T)
//{
//  const unsigned int nx = T.rows();
//  const unsigned int ny = T.cols();
//  ::BeginDrawing();
//    ::ClearBackground((Color){ 255, 255, 255, 255});
//    for(unsigned int i = 0; i < nx; ++i) {
//      for(unsigned int k = 0; k < ny; ++k) {
//        ::DrawRectangle((WINDOW_WIDTH/2 - nx*PIXEL/2) + i*PIXEL, 
//                       (WINDOW_HEIGHT/2 + ny*PIXEL/2) - k*PIXEL, 
//                       PIXEL, PIXEL, m_TtoC(cold_temp, hot_temp, T(i, k)));
//      }
//    }
  
//    int posY = 5;
//    for(unsigned int i = 0; i < jet.size(); ++i) {
//      ::DrawRectangle(5, posY, 5, 1, jet[i]);
//      posY += 1;
//    }
//  ::EndDrawing();
//}

//! This function returns the estimated time to the end of the simulation.
/*!
 While performing the calculation, parameters of simulation_state_t are set.
 \param n_calls number of steps the simulation has run.
 \sa simulation_state_t.
*/
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
      simulation_state.total = m_END_CICLE;
      simulation_state.step = n_calls;
      simulation_state.eta = delta_t * (m_END_CICLE - n_calls);
      simulation_state.velocity = 1.0/delta_t;
      std::cerr << "INFO: RayBenConvection: m_ETA: eta = "
                << std::setw(4) << std::setprecision(3)
                << simulation_state.eta << "s\tspeed = "
                << simulation_state.velocity << "cicle/s\tstep = "
                << n_calls << "/" << simulation_state.total << std::endl;
      ::fflush(stdout);
    }
  }
  old_time = std::chrono::high_resolution_clock::now();
  return;
}

//! This function returns an Eigen sparce diagonal matrix.
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

//! This function returns the Kronecher product of two constant Eigen matrices.
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

//! This function converts Eigen sparce matrix to an std vector of Eigen Triplets.
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

//! This function returns a permutation vector.
/*! 
 Calling p = symamd(S) returns the permutation vector p such that S(p,p)
 tends to have a sparser Cholesky factor than S.
 \param mat must be a symmetric positive definite Eigen matrix.
*/
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

  ::memset(A, 0, (spmat.nonZeros()) * sizeof(int));
  ::memset(p, 0, (mat.cols()+1) * sizeof(int));
  ::memset(perm, 0, (mat.cols()+1) * sizeof(int));
  ::memset(stats, 0, COLAMD_STATS * sizeof(int));

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
//**************************************** TODO non cancellare gli spazi qui sotto perf.

//! This function resolves Partial Pivoting calculations with LU decomposition.
/*!
 Calling this function will return an std vector of Eigen Matrices
 consisting of the two matrices L and U obtained during the decomposition.                                  
 The input matrix A is decomposed as A = PLU where L is unit-lower-triangular,
 U is upper-triangular, and P is a permutation matrix.
 \param mat must be a square invertible matrix.
 \param ret_vett ret_vett[0] = PL matrix, ret_vett[1] = U matrix.
*/
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

//! This function writes in the output file the m_T(i, j) matrix.
/*!
 The output is in C format.
 \sa write_current_data()
*/
void RayBenConvection::m_write_current_frame (){
  m_output_header_file << "{";
    for (unsigned int i = 0; i < m_ny; i++) {
      m_output_header_file << "{";
        for (unsigned int j = 0; j < m_nx; j++)
          if (j < m_nx - 1)
            m_output_header_file << "\t" << m_T(j, i) << ", ";
          else m_output_header_file << "\t" << m_T(j, i); 
      m_output_header_file << "},\n ";
      }
  m_output_header_file << "}";
};

//! This function initialises the private members of the function from the users' input.
/*!
 The parameter inserted by the user are thoughtfully checked to ensure the validity of the simulation.
 \param END_CICLE total numer of steps.
 \param cold_temp lower wall temperature. 
 \param hot_temp higher wall temperature.
 \param Ray_numb Rayleigh number.
 \param Pr_numb Prandtl number.
 \param Re_numb Reynolds number.
*/
int RayBenConvection::init(unsigned int END_CICLE, double cold_temp, double hot_temp, 
                            double Ray_numb, double Pr_numb, double Re_numb ){

  std::cerr << "INFO: RayBenConvection: init: Initializing simulation..." << std::endl;
  std::chrono::time_point<std::chrono::high_resolution_clock> init_lenght = 
    std::chrono::high_resolution_clock::now();
  Eigen::setNbThreads(NPROC);

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
    std::cerr << "ERROR: RayBenConvection: init: The number of steps requested "
              << END_CICLE << " is too high." << std::endl;
    return -1;
  }

  if(m_TS != m_TN)
    m_To = (m_TS < m_TN) ? m_TS : m_TN;
  else{
    std::cout << "Please, remeber to provide two different temperatures.";
    return -2;
  }

  if (m_TN <= 0 || m_TN >= 100){
    std::cout << "Default parameters regard liquid water. Work in 1-99 range.";
    return -3;
  }

  if (m_TS >= 100){
    std::cout << "Default parameters regard liquid water. Work in 1-99 range.";
    return -7;
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

  //initialising calculation matrices:
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

  //%%
  //%Evaluating temperature and velocity field at each time step

  //LU decomposition calcolation:
  m_Lu_solver.compute(m_LUu[0]);

  m_Uu_solver.compute(m_LUu[1]);

  m_Lv_solver.compute(m_LUv[0]);

  m_Uv_solver.compute(m_LUv[1]);

  m_Lp_solver.compute(m_LUp[0]);

  m_Up_solver.compute(m_LUp[1]);

  m_Lt_solver.compute(m_LUt[0]);

  m_Ut_solver.compute(m_LUt[1]);

  std::cerr << "INFO: RayBenConvection: init: Done! Time spent during initialization function: "
  << static_cast<float>(
       std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - init_lenght
       ).count()
     ) / 1000
  << "s" << std::endl;

  m_it=0;


  return ret_value;

}

//! This function performs the perturbative calculations.
RayBenConvection::simulation_state_t RayBenConvection::eval_next_frame(){

  if (m_it < m_END_CICLE ) {

    m_ETA(m_it);

    m_apply_correction();


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
    std::cerr << "INFO: RayBenConvection: eval_next_frame: " 
              << "Simulation ended with no errors." << std::endl;
    simulation_state.ended = true;
    return simulation_state;
  }

  ++m_it;

  simulation_state.ended = false;
  return simulation_state;
}

//! This function writes on the output header file the values of the m_T matrix's elements computed during the simulation.
/*!
This public function creates the C format output. It is called once in every eval_next_frame iteration,
and it checks if the simulation has reached its end. 
\sa eval_next_frame()
\sa m_write_current_frame()
*/

void RayBenConvection::write_current_data(){
  if (m_it == 0) {
    m_output_header_file.open(OUTPUT_HEADER_FILE_NAME, std::ios_base::out);
    std::cerr << "INFO: RayBenConvection: m_write_current_data: opened data file"
              << std::endl;
    if (! m_output_header_file.is_open()){
      std::cerr <<"ERROR: RayBenConvection: Failed opening data file. Exiting..."
                << std::endl;
      return;
    }
    m_output_header_file << "#define NX " << m_nx << "\n" 
                            "#define NY " << m_ny << "\n"
                            "#define N_STEPS " << m_END_CICLE << "\n" 
                            "const double cold_temp = " << m_TN << ";\n"
                            "const double hot_temp = " << m_TS << ";\n"
                            "const double T [N_STEPS][NY][NX] = {\n";
    m_write_current_frame();
  }
  else if (m_it == m_END_CICLE){
    m_output_header_file << "}};/*here*/" << std::endl;
    m_output_header_file.close();
    std::cerr << "INFO: RayBenConvection: m_write_current_data: closed data file"
              << std::endl;
  }
  else {
    m_output_header_file <<",\n";
    m_write_current_frame();
  }
}

#include "args.h"
#include "kernel.h"
#include "verify.h"
using namespace exafmm;

int main(int argc, char ** argv) {
  Args args(argc, argv);
  P = args.P;
  verbose = args.verbose;
  initKernel();

  //! P2M
  Bodies jbodies(1);
  jbodies[0].X = 2;
  jbodies[0].q = 1;
  Cells cells(4);
  Cell * Cj = &cells[0];
  Cj->X[0] = 3;
  Cj->X[1] = 1;
  Cj->X[2] = 1;
  Cj->R = 1;
  Cj->BODY = &jbodies[0];
  Cj->NBODY = jbodies.size();
  Cj->M.resize(NTERM, 0.0);
  P2M(Cj);

  //! M2M
  Cell * CJ = &cells[1];
  CJ->CHILD = Cj;
  CJ->NCHILD = 1;
  CJ->X[0] = 4;
  CJ->X[1] = 0;
  CJ->X[2] = 0;
  CJ->R = 2;
  CJ->M.resize(NTERM, 0.0);
  M2M(CJ);

  //! M2L
  Cell * CI = &cells[2];
  CI->X[0] = -4;
  CI->X[1] = 0;
  CI->X[2] = 0;
  CI->R = 2;
  CI->L.resize(NTERM, 0.0);
  M2L(CI, CJ);

  //! L2L
  Cell * Ci = &cells[3];
  CI->CHILD = Ci;
  CI->NCHILD = 1;
  Ci->X[0] = -3;
  Ci->X[1] = 1;
  Ci->X[2] = 1;
  Ci->R = 1;
  Ci->L.resize(NTERM, 0.0);
  L2L(CI);

  //! L2P
  Bodies bodies(1);
  bodies[0].X[0] = -2;
  bodies[0].X[1] = 2;
  bodies[0].X[2] = 2;
  bodies[0].q = 1;
  bodies[0].p = 0;
  bodies[0].F = 0;
  Ci->BODY = &bodies[0];
  Ci->NBODY = bodies.size();
  L2P(Ci);

  //! P2P
  Bodies bodies2(1);
  for (size_t b=0; b<bodies2.size(); b++) {
    bodies2[b] = bodies[b];
    bodies2[b].p = 0;
    bodies2[b].F = 0;
  }
  Cj->NBODY = jbodies.size();
  Ci->NBODY = bodies2.size();
  Ci->BODY = &bodies2[0];
  P2P(Ci, Cj);

  //! Verify results
  Verify verify;
  double pDif = verify.getDifScalar(bodies, bodies2);
  double pNrm = verify.getNrmScalar(bodies2);
  double pRel = std::sqrt(pDif/pNrm);
  double FDif = verify.getDifVector(bodies, bodies2);
  double FNrm = verify.getNrmVector(bodies2);
  double FRel = std::sqrt(FDif/FNrm);
  printf("%-20s : %8.5e \n","Rel. L2 Error (p)", pRel);
  printf("%-20s : %8.5e \n","Rel. L2 Error (F)", FRel);
  return 0;
}

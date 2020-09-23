
#include "pressio_solvers.hpp"
#include "./problems/eigen_rosenbrock_N4.hpp"

int main()
{
  std::string sentinel = "PASSED";

  using namespace pressio;

  Eigen::Vector4d state;

  using problem_t = solvers::test::EigenRosenbrock4;
  using state_t   = typename problem_t::state_type;
  using hessian_t = containers::DenseMatrix<Eigen::MatrixXd>;

  problem_t problem;
  state_t x(4);
  x[0] = -0.05; x[1] = 1.1; x[2] = 1.2; x[3] = 1.5;

  using lin_tag      = solvers::linear::direct::HouseholderQR;
  using lin_solver_t = solvers::linear::Solver<lin_tag, hessian_t>;
  lin_solver_t linSolver;

  // using solver = pressio::solvers::nonlinear::composeGaussNewton_t<
  //   problem_t, lin_solver_t>;
  // solver GNSolver(problem, x, linSolver);
  auto GNSolver = pressio::solvers::nonlinear::createGaussNewton(problem,x,linSolver);

  GNSolver.setTolerance(1e-5);
  GNSolver.solve(problem, x);
  state = *x.data();
  std::cout << std::setprecision(14) << state << std::endl;

  std::vector<double> gold = {1.00000001567414e+00, 
              9.99999999124769e-01,
  			      9.99999996519930e-01, 
              9.99999988898883e-01};

  const auto e1 = std::abs(state(0) - gold[0]);
  const auto e2 = std::abs(state(1) - gold[1]);
  const auto e3 = std::abs(state(2) - gold[2]);
  const auto e4 = std::abs(state(3) - gold[3]);
  if (e1>1e-6 or e2>1e-6  or e3>1e-6 or e4>1e-6){
    sentinel = "FAILED";
  }
  std::cout << sentinel << std::endl;
}

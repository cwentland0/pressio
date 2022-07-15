
#include <gtest/gtest.h>
#include "pressio/type_traits.hpp"
#include "pressio/ops.hpp"
#include "pressio/rom_tmp.hpp"

struct MyFom
{
  using time_type = double;
  using state_type = Eigen::VectorXd;
  using right_hand_side_type = state_type;
  int N_ = {};

  MyFom(int N): N_(N){}

  state_type createState() const{
    state_type s(N_);
    s.setConstant(0);
    return s;
  }

  right_hand_side_type createRightHandSide() const{
    right_hand_side_type r(N_);
    r.setConstant(0);
    return r;
  }

  void rightHandSide(const state_type & u,
         const time_type evalTime,
         right_hand_side_type & f) const
  {
    for (decltype(f.rows()) i=0; i<f.rows(); ++i){
      f(i) = u(i) + evalTime;
    }
  }

  Eigen::MatrixXd createApplyJacobianResult(const Eigen::MatrixXd & A) const{
    return Eigen::MatrixXd(N_, A.cols());
  }

  void applyJacobian(const state_type & s,
		     const Eigen::MatrixXd & A,
		     const time_type & evaltime,
		     Eigen::MatrixXd & result) const
  {
    std::cout << "gigi\n";
  }
};

struct NonLinearSolver{
  template<class SystemType, class StateType>
  void solve(const SystemType & system, StateType & state)
  {}
};

TEST(rom_galerkin, test4)
{
  pressio::log::initialize(pressio::logto::terminal);
  pressio::log::setVerbosity({pressio::log::level::debug});

  // create fom
  constexpr int N = 10;
  using fom_t = MyFom;
  fom_t fomSystem(N);

  // create trial space
  using basis_t = Eigen::MatrixXd;
  basis_t phi(N, 3);
  phi.col(0).setConstant(0.);
  phi.col(1).setConstant(1.);
  phi.col(2).setConstant(2.);

  using latent_state_type = Eigen::VectorXd;
  using full_state_type = typename fom_t::state_type;
  auto trialSpace = pressio::rom::create_linear_subspace<latent_state_type, full_state_type>(phi);

  auto romState = trialSpace.createLatentState();
  romState[0]=0.;
  romState[1]=1.;
  romState[2]=2.;

  const auto odeScheme = pressio::ode::StepScheme::BDF1;
  namespace gal = pressio::rom::galerkin;
  auto problem = gal::create_default_implicit_problem(odeScheme, trialSpace, fomSystem);

  using time_type = typename fom_t::time_type;
  const time_type dt = 1.;
  NonLinearSolver solver;
  pressio::ode::advance_n_steps(problem, romState, time_type{0}, dt,
        ::pressio::ode::StepCount(2), solver);
  std::cout << romState << std::endl;

  // fix all checks
  EXPECT_TRUE(false);

  pressio::log::finalize();
}

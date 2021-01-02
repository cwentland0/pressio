
#include <gtest/gtest.h>
#include "pressio_ode.hpp"

namespace{
struct MyApp
{
  using scalar_type   = double;
  using state_type    = Eigen::VectorXd;
  using velocity_type = state_type;
  using jacobian_type = Eigen::SparseMatrix<double>;

public:
  velocity_type createVelocity() const{
    velocity_type f(3);
    return f;
  }

  jacobian_type createJacobian() const{
    jacobian_type J(3,3);
    return J;
  }

  void velocity(const state_type & yn,
		const scalar_type& time,
		velocity_type & f) const
  {
    std::cout << "f: t=" << time << "\n";
    f[0] = time+1.;
    f[1] = time+2.;
    f[2] = time+3.;
  }

  void jacobian(const state_type&, const scalar_type&, jacobian_type&) const{
    // not used by the test
  }
};

struct MyFakeSolver
{
  int count_ = {};

  template<typename system_t, typename state_t>
  void solve(const system_t & sys, state_t & state)
  {
    ++count_;

    state_t R(3);
    sys.residual(state, R);

    if (count_==1){
      // this is called from auxiliary stepper which is bdf1

      // here we should have:
      // R = y_n - y_n-1 - dt*f()
      // computedd at t_1 = dt = 1.1
      // where y_n = y_n-1 = [1.5 1.5 1.5]
      // f = [2.1 3.1 4.1]
      EXPECT_DOUBLE_EQ(R(0), 1.5-1.5-1.1*2.1);
      EXPECT_DOUBLE_EQ(R(1), 1.5-1.5-1.1*3.1);
      EXPECT_DOUBLE_EQ(R(2), 1.5-1.5-1.1*4.1);
    }
    if (count_==2)
    {
      // this is called from bdf2
      // here we should have:
      // R = y_n+1 - (4/3)y_n + (1/3)*y_n-1 - (2/3)*dt*f()
      // computedd at t_2 = dt = 2.2
      // where y_n+1 = y_n = y_n-1 = [1.5 1.5 1.5]
      // f = [3.2 4.2 5.2]
      //
      std::cout << *state.data() << std::endl;
      std::cout << *R.data() << std::endl;
      EXPECT_DOUBLE_EQ(R(0), 1.5-(4./3.)*1.5+(1./3.)*1.5 - (2./3.)*1.1*3.2);
      EXPECT_DOUBLE_EQ(R(1), 1.5-(4./3.)*1.5+(1./3.)*1.5 - (2./3.)*1.1*4.2);
      EXPECT_DOUBLE_EQ(R(2), 1.5-(4./3.)*1.5+(1./3.)*1.5 - (2./3.)*1.1*5.2);
    }
  }
};
}

TEST(ode_implicit_bdf2, appVelocityCalledWithCorrectTime)
{
  // fake test to make sure the app velocity
  // for bdf1 is called with the time at the next time step
  // so here we set the velocity to be time+something
  // so that when the "fake" solver is called we can verify
  // that the residual has the correct numbers

  using namespace pressio;
  using app_t = MyApp;
  using nstate_t = typename app_t::state_type;
  using nveloc_t = typename app_t::velocity_type;
  using njacobian_t = typename app_t::jacobian_type;
  app_t appObj;

  using state_t = containers::Vector<nstate_t>;
  using res_t = containers::Vector<nveloc_t>;
  using jac_t = containers::SparseMatrix<njacobian_t>;
  state_t y(3); y.data()->setConstant(1.5);

  using aux_stepper_t = ode::ImplicitStepper<
    ode::implicitmethods::Euler, state_t, res_t, jac_t, app_t>;
  aux_stepper_t stepperAux(y, appObj);

  using stepper_t = ode::ImplicitStepper<
    ode::implicitmethods::BDF2,
    state_t, res_t, jac_t, app_t, aux_stepper_t>;
  stepper_t stepperObj(y, appObj, stepperAux);

  MyFakeSolver solver;
  double dt = 1.1;
  ode::advanceNSteps(stepperObj, y, 0.0, dt, 2, solver);
}

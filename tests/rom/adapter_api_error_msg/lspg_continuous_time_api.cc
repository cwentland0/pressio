
#include "pressio_rom_lspg.hpp"

struct ValidApp{

  using scalar_type	= double;
  using state_type	= Eigen::VectorXd;
  using velocity_type	= state_type;
  using dense_matrix_type = Eigen::MatrixXd;

public:
#if defined NO_VEL
#else
  void velocity(const state_type &,
		const scalar_type & time,
		velocity_type &) const;
#endif

#if defined NO_APP_J
#else
  void applyJacobian(const state_type &,
		     const dense_matrix_type &,
		     const scalar_type &,
		     dense_matrix_type &) const;
#endif

#if defined NO_VEL_C
#else
  velocity_type createVelocity() const;
#endif

#if defined NO_APP_J_C
#else
  dense_matrix_type createApplyJacobianResult(const dense_matrix_type & B) const;
#endif
};

int main(int argc, char *argv[])
{
  using namespace pressio;
  using app_t	      = ValidApp;
  using scalar_t      = typename app_t::scalar_type;
  using native_state_t= typename app_t::state_type;
  using native_dense_mat = typename app_t::dense_matrix_type;
  using fom_state_t   = pressio::containers::Vector<native_state_t>;

  // these are just randomly set, just for testing
  using lspg_state_t  = pressio::containers::Vector<Eigen::VectorXd>;
  using decoder_jac_t = pressio::containers::MultiVector<native_dense_mat>;
  using decoder_t	= pressio::rom::LinearDecoder<decoder_jac_t, fom_state_t>;

  using ode_name_t = pressio::ode::implicitmethods::Euler;

#if defined DEFAULT
  using lspg_problem = pressio::rom::lspg::impl::composeDefaultProblem
    <ode_name_t, app_t, decoder_t, lspg_state_t>::type;
#endif

  // we should never get here because this test fails
  static_assert(std::is_void<lspg_problem>::value, "");

  return 0;
}

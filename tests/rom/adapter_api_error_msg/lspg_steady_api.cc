
#include "pressio_rom.hpp"

struct ValidApp{
  const int32_t numDof_ = 15;

  using scalar_type	= double;
  using state_type	= Eigen::VectorXd;
  using residual_type	= state_type;
  using dense_matrix_type = Eigen::MatrixXd;

public:
#if defined NO_RES
#else
  void residual(const state_type &, residual_type &) const;
#endif

#if defined NO_APP_J
#else
  void applyJacobian(const state_type &,
		     const dense_matrix_type &,
		     dense_matrix_type &) const;
#endif

#if defined NO_RES_C
#else
  residual_type createResidual() const;
#endif

#if defined NO_APP_J_C
#else
  dense_matrix_type createApplyJacobianResult(const dense_matrix_type &) const;
#endif

#if defined PRECOND
 #if defined NO_APP_PREC_TO_RES
 #else
  void applyPreconditioner(const state_type &, residual_type & r) const;
 #endif

 #if defined NO_APP_PREC_TO_MAT
 #else
  void applyPreconditioner(const state_type &, dense_matrix_type & jac) const;
 #endif
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

  using lspg_problem =
#if defined PRECOND
    pressio::rom::lspg::composePreconditionedProblem<
#else
    pressio::rom::lspg::composeDefaultProblem<
#endif
    app_t, lspg_state_t, decoder_t>::type;

  // we should never get here because this test fails
  static_assert(std::is_void<lspg_problem>::value, "");

  return 0;
}

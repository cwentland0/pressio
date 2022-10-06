#include <gtest/gtest.h>
#include "pressio/type_traits.hpp"
#include "pressio/ops.hpp"

struct MatrixType{};
struct FomStateType{};
struct FomResidualType{};
struct FomJacobianActionResultType{};
struct MaskedResidual{};
struct MaskedJacobianAction{};

struct MyFom
{
  using state_type = FomStateType;
  using residual_type = FomResidualType;

  residual_type createResidual() const { return residual_type{}; }

  FomJacobianActionResultType createResultOfJacobianActionOn(const MatrixType & B) const{
    return FomJacobianActionResultType{};
  }

  void residual(const state_type & u,
		residual_type & r) const{}

  void residualAndJacobianAction(const state_type & state,
				 residual_type & r,
				 const MatrixType & B,
				 FomJacobianActionResultType & A,
				 bool computeJac) const{}
};

namespace pressio{
template<> struct Traits<MatrixType>{
  static constexpr int rank = 2;
  using scalar_type = double;
};

template<> struct Traits<FomStateType>{
  static constexpr int rank = 1;
  using scalar_type = double;
};

template<> struct Traits<FomResidualType>{
  static constexpr int rank = 1;
  using scalar_type = double;
};

template<> struct Traits<FomJacobianActionResultType>{
  static constexpr int rank = 2;
  using scalar_type = double;
};

template<> struct Traits<MaskedResidual>{
  static constexpr int rank = 1;
  using scalar_type = double;
};

template<> struct Traits<MaskedJacobianAction>{
  static constexpr int rank = 2;
  using scalar_type = double;
};
}

namespace pressio { namespace ops{

std::size_t extent(const MatrixType &, int ){ return std::size_t{}; }

FomStateType clone(const FomStateType &){ return FomStateType{}; }
MatrixType clone(const MatrixType &){ return MatrixType{}; }

template<class ScalarType>
void fill(FomStateType & fullState, ScalarType value){}

template<class ScalarType>
void update(FomStateType & y, const ScalarType & alpha,
            const FomStateType & x, const ScalarType & beta){}

template<class AlphaType, class BetaType>
void product(::pressio::nontranspose /*mode*/,
             const AlphaType & alpha,
             const MatrixType & basis,
             const Eigen::VectorXd & operand,
             const BetaType & beta,
             FomStateType & fullState){}

}} // end namespace pressio::ops

#include "pressio/rom_subspaces.hpp"
#include "pressio/rom_lspg_steady.hpp"

struct MyMasker
{
  MaskedResidual createApplyMaskResult(const FomResidualType & /*operand*/) const{
    return MaskedResidual{};
  }

  MaskedJacobianAction createApplyMaskResult(const FomJacobianActionResultType & /*operand*/) const{
    return MaskedJacobianAction{};
  }

  void operator()(const FomResidualType & operand,
		  MaskedResidual & result) const{}

  void operator()(const FomJacobianActionResultType & operand,
		  MaskedJacobianAction & result) const{}
};

struct FakeNonLinSolver
{
  template<class SystemType, class StateType>
  void solve(const SystemType & system, StateType & state)
  {
    auto R = system.createResidual();
    auto J = system.createJacobian();
    system.residualAndJacobian(state, R, J, true);
  }
};

TEST(rom_lspg_steady, test5)
{
  /* this test is compile-time only and checks that
     for masked steady lspg all the various
     layers can have/use different types and things behave as they should

     - fom uses FomStateType, FomResidualType, FomJacobianActionResultType
     - the masker acts on the fom types but produces "MaskedResidual and MaskedJacobianAction"
  */

  pressio::log::initialize(pressio::logto::terminal);
  pressio::log::setVerbosity({pressio::log::level::debug});

  MyFom fomSystem;
  MatrixType phi;

  using reduced_state_type = Eigen::VectorXd;
  typename MyFom::state_type shift;
  auto space = pressio::rom::create_trial_column_subspace<reduced_state_type>(phi, shift, false);

  auto romState = space.createReducedState();
  MyMasker mask;
  auto problem = pressio::rom::lspg::create_steady_problem(space, fomSystem, mask);

  FakeNonLinSolver nonLinSolver;
  nonLinSolver.solve(problem, romState);

  pressio::log::finalize();
}

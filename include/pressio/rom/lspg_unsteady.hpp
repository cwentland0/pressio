
#ifndef PRESSIO_ROM_LSPG_UNSTEADY_HPP_
#define PRESSIO_ROM_LSPG_UNSTEADY_HPP_

#include "./impl/lspg_unsteady_fom_states_manager.hpp"
#include "./impl/lspg_unsteady_fully_discrete_system.hpp"
#include "./impl/lspg_unsteady_rj_policy_default.hpp"
#include "./impl/lspg_unsteady_rj_policy_hypred.hpp"
#include "./impl/lspg_unsteady_mask_decorator.hpp"
#include "./impl/lspg_unsteady_problem.hpp"

namespace pressio{ namespace rom{

namespace impl{
void valid_scheme_for_lspg_else_throw(::pressio::ode::StepScheme name){
  if (   name != ::pressio::ode::StepScheme::BDF1
      && name != ::pressio::ode::StepScheme::BDF2)
  {
    throw std::runtime_error("LSPG currently accepting BDF1 or BDF2");
  }
}
}//end impl

namespace lspg{

template<
  std::size_t TotalNumberOfDesiredStates,
  class TrialSubspaceType,
  class FomSystemType
>
auto create_unsteady_problem(const TrialSubspaceType & trialSpace,
			     const FomSystemType & fomSystem)
{

  static_assert(TotalNumberOfDesiredStates == 2 ||
		TotalNumberOfDesiredStates == 3,
		"TotalNumberOfDesiredState must be 2, 3");

  static_assert(PossiblyAffineTrialColumnSubspace<TrialSubspaceType>::value,
		"TrialSubspaceType does not meet the correct concept");

  static_assert(FullyDiscreteSystemWithJacobianAction<
		FomSystemType, TotalNumberOfDesiredStates, typename TrialSubspaceType::basis_matrix_type>::value,
		"FomSystemType does not meet the FullyDiscreteSystemWithJacobianAction concept");

  static_assert(std::is_same<typename TrialSubspaceType::full_state_type,
		typename FomSystemType::state_type>::value == true,
		"Mismatching fom states");

  using independent_variable_type = typename FomSystemType::time_type;
  using reduced_state_type = typename TrialSubspaceType::reduced_state_type;
  using lspg_residual_type = typename FomSystemType::discrete_residual_type;
  using lspg_jacobian_type = typename TrialSubspaceType::basis_matrix_type;

  using system_type = impl::LspgFullyDiscreteSystem<
    TotalNumberOfDesiredStates, independent_variable_type, reduced_state_type,
    lspg_residual_type, lspg_jacobian_type,
    TrialSubspaceType, FomSystemType>;

  using return_type = impl::LspgUnsteadyProblemFullyDiscreteAPI<
    TotalNumberOfDesiredStates, TrialSubspaceType, system_type>;
  return return_type(trialSpace, fomSystem);
}


template<
  class TrialSubspaceType,
  class FomSystemType
>
auto create_unsteady_problem(::pressio::ode::StepScheme schemeName,
			     const TrialSubspaceType & trialSpace,
			     const FomSystemType & fomSystem)
{

  impl::valid_scheme_for_lspg_else_throw(schemeName);

  static_assert(PossiblyAffineTrialColumnSubspace<TrialSubspaceType>::value,
		"TrialSubspaceType does not meet the correct concept");

  static_assert(SemiDiscreteFomWithJacobianAction<
		FomSystemType, typename TrialSubspaceType::basis_matrix_type>::value,
		"FomSystemType does not meet the SemiDiscreteFomWithJacobianAction concept");

  static_assert(std::is_same<typename TrialSubspaceType::full_state_type,
		typename FomSystemType::state_type>::value == true,
		"Mismatching fom states");

  using independent_variable_type = typename FomSystemType::time_type;
  using reduced_state_type = typename TrialSubspaceType::reduced_state_type;

  /* use the FOM rhs to represent the lspg residual
     and the fom jacobian action on basis as the lspg jacobian */
  using lspg_residual_type = typename FomSystemType::right_hand_side_type;
  using lspg_jacobian_type = typename TrialSubspaceType::basis_matrix_type;

  // defining an unsteady lspg problem boils down to
  // defining a "custom" residual and jacobian policy
  // since for lspg we need to customize how we do time stepping
  using rj_policy_type = impl::LspgUnsteadyResidualJacobianPolicy<
    independent_variable_type, reduced_state_type,
    lspg_residual_type, lspg_jacobian_type,
    TrialSubspaceType, FomSystemType>;

  using return_type = impl::LspgUnsteadyProblemSemiDiscreteAPI<TrialSubspaceType, rj_policy_type>;
  return return_type(schemeName, trialSpace, fomSystem);
}

template<
  class TrialSubspaceType,
  class FomSystemType,
  class HypRedUpdaterType
>
auto create_unsteady_problem(::pressio::ode::StepScheme schemeName,
			     const TrialSubspaceType & trialSpace,
			     const FomSystemType & fomSystem,
			     const HypRedUpdaterType & hypRedUpdater)
{

  impl::valid_scheme_for_lspg_else_throw(schemeName);

  static_assert(PossiblyAffineTrialColumnSubspace<TrialSubspaceType>::value,
		"TrialSubspaceType does not meet the correct concept");

  static_assert(SemiDiscreteFomWithJacobianAction<
		FomSystemType, typename TrialSubspaceType::basis_matrix_type>::value,
		"FomSystemType does not meet the SemiDiscreteFomWithJacobianAction concept");

  static_assert(std::is_same<typename TrialSubspaceType::full_state_type,
		typename FomSystemType::state_type>::value == true,
		"Mismatching fom states");

  using independent_variable_type = typename FomSystemType::time_type;
  using reduced_state_type = typename TrialSubspaceType::reduced_state_type;
  /* use the FOM rhs to represent the lspg residual
     and the fom jacobian action on basis as the lspg jacobian */
  using lspg_residual_type = typename FomSystemType::right_hand_side_type;
  using lspg_jacobian_type = typename TrialSubspaceType::basis_matrix_type;

  // defining an unsteady lspg problem boils down to
  // defining a "custom" residual and jacobian policy
  // since for lspg we need to customize how we do time stepping
  using rj_policy_type = impl::LspgUnsteadyResidualJacobianPolicyHypRed<
    independent_variable_type, reduced_state_type,
    lspg_residual_type, lspg_jacobian_type,
    TrialSubspaceType, FomSystemType, HypRedUpdaterType>;

  using return_type = impl::LspgUnsteadyProblemSemiDiscreteAPI<TrialSubspaceType, rj_policy_type>;
  return return_type(schemeName, trialSpace, fomSystem, hypRedUpdater);
}

template<
  class TrialSubspaceType,
  class FomSystemType,
  class ResidualMaskerType,
  class JacobianActionMaskerType
>
auto create_unsteady_problem(::pressio::ode::StepScheme schemeName,
			     const TrialSubspaceType & trialSpace,
			     const FomSystemType & fomSystem,
			     const ResidualMaskerType & rMasker,
			     const JacobianActionMaskerType & jaMasker)
{

  impl::valid_scheme_for_lspg_else_throw(schemeName);

  static_assert(PossiblyAffineTrialColumnSubspace<TrialSubspaceType>::value,
		"TrialSubspaceType does not meet the correct concept");

  static_assert(SemiDiscreteFomWithJacobianAction<
		FomSystemType, typename TrialSubspaceType::basis_matrix_type>::value,
		"FomSystemType does not meet the SemiDiscreteFomWithJacobianAction concept");

  static_assert(std::is_same<typename TrialSubspaceType::full_state_type,
		typename FomSystemType::state_type>::value == true,
		"Mismatching fom states");

  // masker must be invariant in time
  static_assert(TimeInvariantMasker<ResidualMaskerType>::value,
		"ResidualMaskerType does not meet the TimeInvariantMasker concept");
  static_assert(TimeInvariantMasker<JacobianActionMaskerType>::value,
		"JacobianActionMaskerType does not meet the TimeInvariantMasker concept");

  // ensure the masker acts on the FOM types
  static_assert(std::is_same<
		typename ResidualMaskerType::operand_type,
		typename FomSystemType::right_hand_side_type>::value == true,
		"mismatching types of rhs masker and fom right_hand_side_type");
  using fom_jac_action_result_type =
    decltype(std::declval<FomSystemType const>().createApplyJacobianResult
	     (std::declval<typename TrialSubspaceType::basis_matrix_type const &>()) );
  static_assert(std::is_same<
		typename JacobianActionMaskerType::operand_type,
		fom_jac_action_result_type>::value == true,
		"mismatching types of jacobian action masker and fom");

  using independent_variable_type = typename FomSystemType::time_type;
  using reduced_state_type = typename TrialSubspaceType::reduced_state_type;

  using lspg_unmasked_residual_type = typename FomSystemType::right_hand_side_type;
  using lspg_unmasked_jacobian_type = typename TrialSubspaceType::basis_matrix_type;

  using lspg_residual_type = typename ResidualMaskerType::result_type;
  using lspg_jacobian_type = typename JacobianActionMaskerType::result_type;

  // defining an unsteady lspg problem boils down to
  // defining a "custom" residual and jacobian policy
  // since for lspg we need to customize how we do time stepping
  using rj_policy_type =
    impl::LspgMaskDecorator<
      ResidualMaskerType, JacobianActionMaskerType,
    impl::LspgUnsteadyResidualJacobianPolicy<
      independent_variable_type, reduced_state_type,
      lspg_unmasked_residual_type, lspg_unmasked_jacobian_type,
      TrialSubspaceType, FomSystemType
      >
    >;

  using return_type = impl::LspgUnsteadyProblemSemiDiscreteAPI<TrialSubspaceType, rj_policy_type>;
  return return_type(schemeName, trialSpace, fomSystem, rMasker, jaMasker);
}

}}} // end pressio::rom::lspg
#endif

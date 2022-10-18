/*
//@HEADER
// ************************************************************************
//
// ode_create_implicit_stepper.hpp
//                     		  Pressio
//                             Copyright 2019
//    National Technology & Engineering Solutions of Sandia, LLC (NTESS)
//
// Under the terms of Contract DE-NA0003525 with NTESS, the
// U.S. Government retains certain rights in this software.
//
// Pressio is licensed under BSD-3-Clause terms of use:
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Francesco Rizzi (fnrizzi@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef ODE_ODE_CREATE_IMPLICIT_STEPPER_HPP_
#define ODE_ODE_CREATE_IMPLICIT_STEPPER_HPP_

#include "./impl/ode_implicit_create_impl.hpp"

namespace pressio{ namespace ode{

template<
  class SystemType
#if not defined PRESSIO_ENABLE_CXX20
  ,mpl::enable_if_t<
     ::pressio::ode::SystemWithRhsAndJacobian<mpl::remove_cvref_t<SystemType>>::value, int > = 0
#endif
  >
#ifdef PRESSIO_ENABLE_CXX20
requires ::pressio::ode::SystemWithRhsAndJacobian<mpl::remove_cvref_t<SystemType>>
#endif
auto create_implicit_stepper(StepScheme name,
			     SystemType && system)
{

  using system_type = mpl::remove_cvref_t<SystemType>;
  using independent_variable_type = typename system_type::independent_variable_type;
  using state_type    = typename system_type::state_type;
  using residual_type = typename system_type::right_hand_side_type;
  using jacobian_type = typename system_type::jacobian_type;

  // it is very important to use "SystemType" as template arg
  // because that it the right type carrying how we store the system
  using rj_policy_type = impl::ResidualJacobianStandardPolicy<
    SystemType, independent_variable_type,
    state_type, residual_type, jacobian_type>;

  using impl_type = impl::ImplicitStepperStandardImpl<
    independent_variable_type, state_type, residual_type,
    jacobian_type, rj_policy_type>;

  return impl::create_implicit_stepper_impl<
    impl_type>(name,
	       rj_policy_type(std::forward<SystemType>(system)));
}

template<
  class SystemType
#if not defined PRESSIO_ENABLE_CXX20
  ,mpl::enable_if_t<
     ::pressio::ode::SystemWithRhsJacobianMassMatrix<mpl::remove_cvref_t<SystemType>>::value, int > = 0
#endif
  >
#ifdef PRESSIO_ENABLE_CXX20
requires ::pressio::ode::SystemWithRhsJacobianMassMatrix<mpl::remove_cvref_t<SystemType>>
#endif
auto create_implicit_stepper(StepScheme name,
			     SystemType && system)
{

  using system_type = mpl::remove_cvref_t<SystemType>;
  using independent_variable_type = typename system_type::independent_variable_type;
  using state_type    = typename system_type::state_type;
  using residual_type = typename system_type::right_hand_side_type;
  using jacobian_type = typename system_type::jacobian_type;

  // it is very important to use "SystemType" as template arg
  // because that it the right type carrying how we store the system
  using rj_policy_type = impl::ResidualJacobianWithMassMatrixStandardPolicy<
    SystemType, independent_variable_type,
    state_type, residual_type, jacobian_type>;

  using impl_type = impl::ImplicitStepperStandardImpl<
    independent_variable_type, state_type, residual_type,
    jacobian_type, rj_policy_type>;

  return impl::create_implicit_stepper_impl<
    impl_type>(name,
	       rj_policy_type(std::forward<SystemType>(system)));
}


template<
  class ResidualJacobianPolicyType
#if not defined PRESSIO_ENABLE_CXX20
  ,mpl::enable_if_t<
    ::pressio::ode::ImplicitResidualJacobianPolicy<
      mpl::remove_cvref_t<ResidualJacobianPolicyType>>::value, int
    > = 0
#endif
  >
#ifdef PRESSIO_ENABLE_CXX20
requires ::pressio::ode::ImplicitResidualJacobianPolicy<
  mpl::remove_cvref_t<ResidualJacobianPolicyType>
  >
#endif
auto create_implicit_stepper(StepScheme name,
			     ResidualJacobianPolicyType && policy)
{

  using policy_type = mpl::remove_cvref_t<ResidualJacobianPolicyType>;
  using independent_variable_type  = typename policy_type::independent_variable_type;
  using state_type    = typename policy_type::state_type;
  using residual_type = typename policy_type::residual_type;
  using jacobian_type = typename policy_type::jacobian_type;

  using impl_type = impl::ImplicitStepperStandardImpl<
    independent_variable_type, state_type, residual_type,
    jacobian_type, ResidualJacobianPolicyType>;

  return impl::create_implicit_stepper_impl<
    impl_type>(name, std::forward<ResidualJacobianPolicyType>(policy));
}


// num of states as template arg constructs the arbitrary stepper
template<int TotalNumberOfDesiredStates, class SystemType>
#ifdef PRESSIO_ENABLE_CXX20
requires ::pressio::ode::FullyDiscreteSystemWithJacobian<
  mpl::remove_cvref_t<SystemType>, TotalNumberOfDesiredStates>
#endif
auto create_implicit_stepper(SystemType && system)
{

  using sys_type = mpl::remove_cvref_t<SystemType>;
#if not defined PRESSIO_ENABLE_CXX20
  static_assert(::pressio::ode::FullyDiscreteSystemWithJacobian<
		sys_type, TotalNumberOfDesiredStates>::value,
		"The system passed does not meet the FullyDiscrete API");
#endif

  using sys_type = mpl::remove_cvref_t<SystemType>;
  using independent_variable_type = typename sys_type::independent_variable_type;
  using state_type = typename sys_type::state_type;
  using residual_type = typename sys_type::discrete_residual_type;
  using jacobian_type = typename sys_type::discrete_jacobian_type;

  using stepper_type = impl::StepperArbitrary<
    TotalNumberOfDesiredStates, SystemType, independent_variable_type,
    state_type, residual_type, jacobian_type
    >;

  return stepper_type(std::forward<SystemType>(system));
}

//
// auxiliary API
//
template<class ...Args>
auto create_bdf1_stepper(Args && ... args){
  return create_implicit_stepper(StepScheme::BDF1,
				 std::forward<Args>(args)...);
}

template<class ...Args>
auto create_bdf2_stepper(Args && ... args){
  return create_implicit_stepper(StepScheme::BDF2,
				 std::forward<Args>(args)...);
}

template<class ...Args>
auto create_cranknicolson_stepper(Args && ... args){
  return create_implicit_stepper(StepScheme::CrankNicolson,
				 std::forward<Args>(args)...);
}

}} // end namespace pressio::ode
#endif  // ODE_ODE_CREATE_IMPLICIT_STEPPER_HPP_

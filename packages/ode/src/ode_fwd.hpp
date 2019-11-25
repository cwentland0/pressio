/*
//@HEADER
// ************************************************************************
//
// ode_fwd.hpp
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

#ifndef ODE_FORWARD_DECLARATIONS_HPP_
#define ODE_FORWARD_DECLARATIONS_HPP_

#include "ode_ConfigDefs.hpp"

namespace pressio{ namespace ode{

class nMinusOne{};
class nMinusTwo{};
class nMinusThree{};
class nMinusFour{};


/*--------------------------
 * EXPLICIT METHODS
 --------------------------*/
namespace explicitmethods{

template<
  typename stepper_tag,
  typename state_type,
  typename ...Args
  >
class Stepper;


namespace policy{
template<
  typename state_type,
  typename model_type,
  typename velocity_type = state_type,
  typename enable = void>
class VelocityStandardPolicy;
}//end namespace pressio::ode::explicitmethods::policy


namespace impl {
template<
  typename scalar_type,
  typename state_type,
  typename model_type,
  typename velocity_type,
  typename policy_type,
  typename ops,
  typename enable = void
  >
class ExplicitEulerStepperImpl;

template<
  typename scalar_type,
  typename state_type,
  typename model_type,
  typename velocity_type,
  typename policy_type,
  typename ops,
  typename enable = void
  >
class ExplicitRungeKutta4StepperImpl;
}//end namespace pressio::ode::explicitmethods::impl

}//end namespace pressio::ode::explicitmethods



/*--------------------------
 * IMPLICIT METHODS
 --------------------------*/
namespace implicitmethods{

template<
  typename stepper_tag,
  typename state_type,
  typename residual_type,
  typename jacobian_type,
  typename model_type,
  typename ...Args>
class Stepper;

namespace policy{
template<
  typename state_type,
  typename model_type,
  typename residual_type = state_type,
  typename enable = void>
class ResidualStandardPolicy;

template<
  typename state_type,
  typename model_type,
  typename residual_type = state_type,
  typename enable = void>
class ResidualStandardPolicyForArbitraryStepper;

template<
  typename state_type,
  typename model_type,
  typename jacobian_type,
  typename enable = void>
class JacobianStandardPolicy;

template<
  typename state_type,
  typename model_type,
  typename jacobian_type,
  typename enable = void>
class JacobianStandardPolicyForArbitraryStepper;

#ifdef PRESSIO_ENABLE_TPL_PYBIND11
template<
  typename state_type,
  typename model_type,
  typename residual_type = state_type,
  typename enable = void>
class ResidualStandardPolicyPybind11;

template<
  typename state_type,
  typename model_type,
  typename jacobian_type,
  typename enable = void>
class JacobianStandardPolicyPybind11;
#endif

}//end namespace pressio::ode::implicitmethods::policy

}//end namespace pressio::ode::explicitmethods


// aliases to deprecate later
template<
  ExplicitEnum stepperName,
  typename state_type,
  typename ...Args
  >
using ExplicitStepper = ::pressio::ode::explicitmethods::Stepper<
  typename impl::ExplicitEnumToTagType<stepperName>::type, state_type, Args...
  >;

template<
  ImplicitEnum stepperName,
  typename state_type,
  typename residual_type,
  typename jacobian_type,
  typename model_type,
  typename ...Args
  >
using ImplicitStepper = ::pressio::ode::implicitmethods::Stepper<
  typename impl::ImplicitEnumToTagType<stepperName>::type, state_type, residual_type, jacobian_type, model_type, Args...
  >;

}} // end namespace pressio::ode
#endif

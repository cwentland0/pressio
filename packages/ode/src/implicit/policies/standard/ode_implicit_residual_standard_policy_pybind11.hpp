/*
//@HEADER
// ************************************************************************
//
// ode_implicit_residual_standard_policy_pybind11.hpp
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

#ifdef PRESSIO_ENABLE_TPL_PYBIND11
#ifndef ODE_POLICIES_STANDARD_IMPLICIT_RESIDUAL_STANDARD_POLICY_PYBIND11_HPP_
#define ODE_POLICIES_STANDARD_IMPLICIT_RESIDUAL_STANDARD_POLICY_PYBIND11_HPP_

#include "../../../ode_fwd.hpp"
#include "../base/ode_implicit_residual_policy_base.hpp"
#include "../../ode_time_discrete_residual.hpp"
#include "../../meta/ode_is_legitimate_implicit_state_type.hpp"
#include "../../meta/ode_is_legitimate_implicit_residual_type.hpp"
#include "../../meta/ode_implicit_stepper_stencil_needs_previous_states_only.hpp"
#include "../../meta/ode_implicit_stepper_stencil_needs_previous_states_and_velocities.hpp"


namespace pressio{ namespace ode{ namespace implicitmethods{ namespace policy{

template<
  typename state_type,
  typename system_type,
  typename residual_type
  >
class ResidualStandardPolicyPybind11<
  state_type, system_type, residual_type,
  ::pressio::mpl::enable_if_t<
    ::pressio::ode::meta::is_legitimate_implicit_state_type<state_type>::value and
    ::pressio::ode::meta::is_legitimate_implicit_residual_type<residual_type>::value and
    mpl::is_same<system_type, pybind11::object >::value and
    containers::meta::is_array_pybind11<state_type>::value and
    containers::meta::is_array_pybind11<residual_type>::value
    >
  >
  : public ResidualPolicyBase<
  ResidualStandardPolicyPybind11<state_type, system_type, residual_type>>
{

  using this_t = ResidualStandardPolicyPybind11<state_type, system_type, residual_type>;
  friend ResidualPolicyBase<this_t>;

public:
  ResidualStandardPolicyPybind11() = default;
  ~ResidualStandardPolicyPybind11() = default;

public:

  // TODO

};//end class

}}}}//end namespace pressio::ode::implicitmethods::policy
#endif
#endif

/*
//@HEADER
// ************************************************************************
//
// ode_advance_n_steps_implicit_arbitrary_step_size.hpp
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

#ifndef ODE_INTEGRATORS_ODE_ADVANCE_N_STEPS_IMPLICIT_ARBITRARY_STEP_SIZE_HPP_
#define ODE_INTEGRATORS_ODE_ADVANCE_N_STEPS_IMPLICIT_ARBITRARY_STEP_SIZE_HPP_

#include "./impl/ode_n_steps_integrators.hpp"

namespace pressio{ namespace ode{

// no collector object is passed
template<
  typename stepper_type,
  typename state_type,
  typename time_type,
  typename solver_type,
  typename step_size_cb_t,
  typename ...Args
  >
::pressio::mpl::enable_if_t<
  ::pressio::ode::implicitly_steppable<
    stepper_type, state_type, time_type, solver_type>::value and
  ::pressio::ode::time_step_size_manager<
    step_size_cb_t, ::pressio::ode::step_count_type, time_type>::value and
  ::pressio::ode::legitimate_solver_for_implicit_stepper<
    solver_type, stepper_type, state_type>::value
  >
advanceNSteps(stepper_type & stepper,
	      state_type & odeStateInOut,
	      const time_type start_time,
	      const ::pressio::ode::step_count_type num_steps,
	      step_size_cb_t && dtManager,
	      solver_type & solver,
	      Args && ...solver_args)
{

  static_assert(::pressio::ode::implicit_state<state_type>::value,
		"You are trying to call advanceNSteps with an implicit stepper \
but the state type you are using is not admissible for implicit time-stepping. ");

  using advancer_t  = impl::IntegratorNStepsWithTimeStepSizeSetter;
  using collector_t = ::pressio::ode::impl::DummyCollector<time_type, state_type>;
  collector_t collector;
  advancer_t::execute(stepper, num_steps, start_time, odeStateInOut,
		      std::forward<step_size_cb_t>(dtManager),
		      collector, solver,
		      std::forward<Args>(solver_args)...);
}

// a collector object is passed
template<
  typename stepper_type,
  typename state_type,
  typename time_type,
  typename step_size_cb_type,
  typename collector_type,
  typename solver_type,
  typename ...Args
  >
::pressio::mpl::enable_if_t<
  ::pressio::ode::implicitly_steppable<
    stepper_type, state_type, time_type, solver_type>::value and
  ::pressio::ode::time_step_size_manager<
    step_size_cb_type, ::pressio::ode::step_count_type, time_type>::value and
  ::pressio::ode::collector<
    collector_type, time_type, state_type>::value and
  ::pressio::ode::legitimate_solver_for_implicit_stepper<
    solver_type, stepper_type, state_type>::value
  >
advanceNSteps(stepper_type & stepper,
	      state_type & odeStateInOut,
	      const time_type start_time,
	      const ::pressio::ode::step_count_type num_steps,
	      step_size_cb_type && dtManager,
	      collector_type & collector,
	      solver_type & solver,
	      Args && ...solver_args)
{

  static_assert(::pressio::ode::implicit_state<state_type>::value,
		"You are trying to call advanceNSteps with an implicit stepper \
but the state type you are using is not admissible for implicit time-stepping. ");

  using advancer_t  = impl::IntegratorNStepsWithTimeStepSizeSetter;
  advancer_t::execute(stepper, num_steps, start_time, odeStateInOut,
		      std::forward<step_size_cb_type>(dtManager),
		      collector, solver,
		      std::forward<Args>(solver_args)...);
}

}}//end namespace pressio::ode
#endif  // ODE_INTEGRATORS_ODE_ADVANCE_N_STEPS_IMPLICIT_ARBITRARY_STEP_SIZE_HPP_

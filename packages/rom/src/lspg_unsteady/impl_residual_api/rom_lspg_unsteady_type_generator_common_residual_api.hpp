/*
//@HEADER
// ************************************************************************
//
// rom_lspg_unsteady_type_generator_common_residual_api.hpp
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

#ifndef ROM_LSPG_UNSTEADY_TYPE_GENERATOR_COMMON_RESIDUAL_API_HPP_
#define ROM_LSPG_UNSTEADY_TYPE_GENERATOR_COMMON_RESIDUAL_API_HPP_

#include "../../rom_fwd.hpp"
#include "../../meta/rom_is_legitimate_decoder_type.hpp"
#include "../impl_shared/rom_lspg_unsteady_aux_stepper_type_helper.hpp"
#include "../impl_shared/rom_lspg_unsteady_fom_states_storage_capacity_helper.hpp"
#include "../../../../ode/src/implicit/meta/ode_is_stepper_order_setter.hpp"
#include "../../../../ode/src/implicit/meta/ode_is_stepper_total_n_states_setter.hpp"

namespace pressio{ namespace rom{ namespace impl{

template <
  ::pressio::ode::ImplicitEnum odeName,
  typename fom_type,
  typename lspg_state_type,
  typename ...Args
  >
struct LSPGUnsteadyCommonTypesResidualApi
{
  //::pressio::containers::meta::is_vector_wrapper<lspg_state_type>::value

  static_assert( ::pressio::rom::meta::model_meets_residual_api_for_unsteady_lspg<fom_type>::value,
		 "\nYou are trying to setup an unsteady LSPG problem requiring \n \
a fom adapter class to meet the residual api. \n \
However, the fom/adapter type you passed does not meet that api. \n \
Verify the fom/adapter class to check if you are missing something.");

  // verify that args contains a valid decoder type
  using ic0 = ::pressio::mpl::variadic::find_if_unary_pred_t<
    ::pressio::rom::meta::is_legitimate_decoder_type, Args...>;
  using decoder_t = ::pressio::mpl::variadic::at_or_t<void, ic0::value, Args...>;
  static_assert(!std::is_void<decoder_t>::value and ic0::value < sizeof... (Args),
		"A valid decoder type must be passed to define a LSPG problem");
  using decoder_jac_t = typename decoder_t::jacobian_t;

  // these are native types of the full-order model (fom)
  using fom_t			= fom_type;
  using scalar_t		= typename fom_t::scalar_type;
  using fom_native_state_t	= typename fom_t::state_type;
  using fom_native_residual_t	= typename fom_t::residual_type;

  // fom wrapper types
  using fom_state_t		= ::pressio::containers::Vector<fom_native_state_t>;
  using lspg_residual_t		= ::pressio::containers::Vector<fom_native_residual_t>;

  // rom state type (passed in)
  using lspg_state_t		= lspg_state_type;

  /* lspg_matrix_t is type of J*decoder_jac_t (in the most basic case) where
   * * J is the jacobian of the fom rhs
   * * decoder_jac_t is the type of the decoder jacobian
   * In more complex cases, we might have (something)*J*decoder_jac_t,
   * where (something) is product of few matrices.
   * For now, set lspg_matrix_t to be of same type as decoder_jac_t
   * if phi is MV<>, then lspg_matrix_t = containers::MV<>
   * if phi is Matrix<>, then we have containers::Matrix<>
   * not bad assumption since all matrices are left-applied to decoder_jac_t
   */
  using lspg_matrix_t		= decoder_jac_t;

  // fom state reconstructor type
  using fom_state_reconstr_t	= FomStateReconstructor<fom_state_t, decoder_t>;

  //-------------------------------
  // find the order setter in Args
  //-------------------------------
  using ic1 = ::pressio::mpl::variadic::find_if_unary_pred_t<
    ::pressio::ode::meta::impl::IsStepperOrderSetter, Args...>;
  using order_setter = ::pressio::mpl::variadic::at_or_t<void, ic1::value, Args...>;
  static_assert( !std::is_void<order_setter>::value,
  		 "To use lspg with residual api, you need to set the order of the stepper \n \
at compile time by passing to LSPGUnsteadyProblem a template argument as follows: \n \
::pressio::ode::types::StepperOrder<your_order_value>.");
  // store
  static constexpr ::pressio::ode::types::stepper_order_t order_value = order_setter::value;

  //-----------------------------------------------------------
  // find the total number of states needed
  //-----------------------------------------------------------
  using ic2 = ::pressio::mpl::variadic::find_if_unary_pred_t<
    ::pressio::ode::meta::impl::IsStepperTotalNumStatesSetter, Args...>;
  using tot_n_setter = ::pressio::mpl::variadic::at_or_t<void, ic2::value, Args...>;
  static_assert( !std::is_void<tot_n_setter>::value,
  		 "\nTo use lspg with residual api, you need to set the \
total number of states needed for the stepper at compile time by passing \
to LSPGUnsteadyProblem a template argument as follows: \n \
::pressio::ode::types::StepperTotalNumberOfStates<your_order_value>. \n \
Note that this is the total number of states needed including previous ones, \n \
basically the size of the stpper stencil.");

  // numAuxStates is the number of auxiliary states needed, so all other beside y_n
  static constexpr std::size_t auxStates = tot_n_setter::value - 1;

  // type of class holding the fom states
  using fom_states_data = ::pressio::rom::FomStatesContainer<fom_state_t, auxStates, fom_state_reconstr_t>;

  // if we have a non-trivial user-defined ops, need to find from Args
  using ud_ops_t = void;
};

}}}//end  namespace pressio::rom::impl
#endif

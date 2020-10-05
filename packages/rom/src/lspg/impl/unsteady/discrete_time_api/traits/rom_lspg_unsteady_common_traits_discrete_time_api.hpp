/*
//@HEADER
// ************************************************************************
//
// rom_lspg_unsteady_common_traits_discrete_time_api.hpp
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

#ifndef ROM_LSPG_IMPL_UNSTEADY_DISCRETE_TIME_API_TRAITS_ROM_LSPG_UNSTEADY_COMMON_TRAITS_DISCRETE_TIME_API_HPP_
#define ROM_LSPG_IMPL_UNSTEADY_DISCRETE_TIME_API_TRAITS_ROM_LSPG_UNSTEADY_COMMON_TRAITS_DISCRETE_TIME_API_HPP_

#include "../../shared/rom_lspg_unsteady_aux_stepper_type_helper.hpp"
#include "../../shared/rom_lspg_unsteady_fom_states_storage_capacity_helper.hpp"
#include "../../shared/rom_lspg_unsteady_fom_state_reconstructor_helper.hpp"

namespace pressio{ namespace rom{ namespace lspg{ namespace impl{ namespace unsteady{

template <
  typename stepper_tag,
  typename fom_system_type,
  typename lspg_state_type,
  typename decoder_type,
  typename ...Args
  >
struct CommonTraitsDiscreteTimeApi
{
  // the scalar type
  using scalar_t = typename ::pressio::containers::details::traits<lspg_state_type>::scalar_t;

  using fom_system_t	      = fom_system_type;
  using fom_native_state_t    = typename fom_system_t::state_type;
  using fom_native_residual_t = typename fom_system_t::discrete_time_residual_type;

  // fom wrapper types
  using fom_state_t		= ::pressio::containers::Vector<fom_native_state_t>;
  using lspg_residual_t		= ::pressio::containers::Vector<fom_native_residual_t>;

  // rom state type (passed in)
  using lspg_state_t		= lspg_state_type;

  // // verify that args contains a valid decoder type
  // using ic0 = ::pressio::mpl::variadic::find_if_ternary_pred_t<
  //   lspg_state_t, fom_state_t, ::pressio::rom::concepts::decoder, Args...>;
  // using decoder_t = ::pressio::mpl::variadic::at_or_t<void, ic0::value, Args...>;
  // static_assert(!std::is_void<decoder_t>::value and ic0::value < sizeof... (Args),
  // 		"A valid decoder type must be passed to define a LSPG problem");
  static_assert
  (::pressio::rom::concepts::decoder<decoder_type, lspg_state_t, fom_state_t>::value,
   "A valid decoder type must be passed to define a LSPG problem");
  using decoder_t = decoder_type;
  using decoder_jac_t = typename decoder_t::jacobian_type;


  /* lspg_matrix_t is type of J*decoder_jac_t (in the most basic case) where
   * * J is the jacobian of the fom rhs
   * * decoder_jac_t is the type of the decoder jacobian
   * In more complex cases, we might have (something)*J*decoder_jac_t,
   * where (something) is product of few matrices.
   * For now, set lspg_matrix_t to be of same type as decoder_jac_t
   * if phi is MV<>, then lspg_matrix_t = containers::MV<>
   * if phi is DenseMatrix<>, then we have containers::DenseMatrix<>
   * not bad assumption since all matrices are left-applied to decoder_jac_t
   */
  using lspg_matrix_t		= decoder_jac_t;

  // if we have an admissible user-defined ops
  using icUdOps = ::pressio::mpl::variadic::find_if_quaternary_pred_t<
    decoder_jac_t, lspg_state_t, fom_state_t,
    ::pressio::rom::concepts::custom_ops_lspg_discrete_time, Args...>;
  using ud_ops_t = ::pressio::mpl::variadic::at_or_t<void, icUdOps::value, Args...>;

  // fom state reconstructor type
  using fom_state_reconstr_t =
    typename FomStateReconHelper<ud_ops_t>::template type<scalar_t, fom_state_t, decoder_t>;

  //-------------------------------
  // find the order setter in Args
  //-------------------------------
  using ic1 = ::pressio::mpl::variadic::find_if_unary_pred_t<
    ::pressio::ode::predicates::IsStepperOrderSetter, Args...>;
  using order_setter = ::pressio::mpl::variadic::at_or_t<void, ic1::value, Args...>;
  static_assert
  ( !std::is_void<order_setter>::value,
    "To use lspg with residual api, you need to set the order of the stepper \n \
at compile time by passing to LSPGUnsteadyProblem a template argument as follows: \n \
::pressio::ode::types::StepperOrder<your_order_value>.");
  // store
  static constexpr ::pressio::ode::types::stepper_order_t order_value = order_setter::value;

  //-----------------------------------------------------------
  // find the total number of states needed
  //-----------------------------------------------------------
  using ic2 = ::pressio::mpl::variadic::find_if_unary_pred_t<
    ::pressio::ode::predicates::IsStepperTotalNumStatesSetter, Args...>;
  using tot_n_setter = ::pressio::mpl::variadic::at_or_t<void, ic2::value, Args...>;
  static_assert( !std::is_void<tot_n_setter>::value,
  		 "\nTo use lspg with residual api, you need to set the \
total number of states needed for the stepper at compile time by passing \
to LSPGUnsteadyProblem a template argument as follows: \n \
::pressio::ode::types::StepperTotalNumberOfStates<your_order_value>. \n \
Note that this is the total number of states needed including previous ones, \n \
basically the size of the stpper stencil.");

  // total number fom states needed (size of stencil plus the state at current step)
  static constexpr std::size_t numStates = tot_n_setter::value;

  // type of class holding the fom states
  using fom_states_manager_t =
    ::pressio::rom::ManagerFomStatesStatic<fom_state_t, numStates,
					   fom_state_reconstr_t, ud_ops_t>;

};

}}}}}//end  namespace pressio::rom::lspg::unsteady::impl
#endif  // ROM_LSPG_IMPL_UNSTEADY_DISCRETE_TIME_API_TRAITS_ROM_LSPG_UNSTEADY_COMMON_TRAITS_DISCRETE_TIME_API_HPP_

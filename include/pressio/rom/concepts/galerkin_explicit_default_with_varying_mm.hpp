/*
//@HEADER
// ************************************************************************
//
// rom_fom_system_continuous_time.hpp
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

#ifndef ROM_CONCEPTS_GALERKIN_EXPLICIT_DEFAULT_WITH_VARYING_MASS_MATRIX_HPP_
#define ROM_CONCEPTS_GALERKIN_EXPLICIT_DEFAULT_WITH_VARYING_MASS_MATRIX_HPP_

#include "helpers.hpp"

#ifdef PRESSIO_ENABLE_CXX20

// this is here so that we can clearly show it in the
// doc via rst literal include directive
namespace pressio{ namespace rom{ namespace galerkin{ namespace unsteadyexplicit{

template <class TrialSubspaceType, class FomSystemType, class FomMassMatrixOperatorType>
concept ComposableIntoDefaultWithVaryingMassMatrixProblem =
     ComposableIntoDefaultProblem<TrialSubspaceType, FomSystemType>
  && std::same_as<
       typename TrialSubspaceType::full_state_type,
       typename FomSystemType::state_type>

  && requires(const FomMassMatrixOperatorType & fomMassMatOp,
	      const TrialSubspaceType & subspace)
  {
    { fomMassMatOp.createApplyMassMatrixResult(subspace.basisOfTranslatedSpace()) } -> std::copy_constructible;
  }
  && requires(const TrialSubspaceType & subspace,
	      const concepts::impl::fom_mass_matrix_action_t<
	        FomMassMatrixOperatorType,
	        concepts::impl::fom_mass_matrix_action_on_trial_space_t<
	           FomMassMatrixOperatorType, TrialSubspaceType>
	      > & fomMassMatrixAction,
	      impl::explicit_galerkin_default_reduced_mass_matrix_t<TrialSubspaceType> & massMatGal,
	      scalar_trait_t<typename TrialSubspaceType::basis_matrix_type>  alpha,
	      scalar_trait_t<typename TrialSubspaceType::reduced_state_type> beta)
  {
    pressio::ops::product
       (::pressio::transpose(), ::pressio::nontranspose(),
	alpha, subspace.basisOfTranslatedSpace(), fomMassMatrixAction,
	beta, massMatGal);
  };

}}}} //end namespace pressio::rom::galerkin::unsteadyexplicit






/* leave some white space on purpose so that
   if we make edits above, we don't have to change
   the line numbers included in the rst doc page */

#else

namespace pressio{ namespace rom{ namespace galerkin{ namespace unsteadyexplicit{

template <
  class TrialSubspaceType, class FomSystemType, class FomMassMatrixOperatorType,
  class = void>
struct ComposableIntoDefaultWithVaryingMassMatrixProblem : std::false_type{};

template <class TrialSubspaceType, class FomSystemType, class FomMassMatrixOperatorType>
struct ComposableIntoDefaultWithVaryingMassMatrixProblem<
  TrialSubspaceType, FomSystemType, FomMassMatrixOperatorType,
  mpl::enable_if_t<
    ComposableIntoDefaultProblem<TrialSubspaceType, FomSystemType>::value
    && std::is_copy_constructible<
      decltype
      (std::declval<FomMassMatrixOperatorType const>().createApplyMassMatrixResult
       (
         std::declval<typename TrialSubspaceType::basis_matrix_type const &>()
       )
       )
    >::value
    //
    && std::is_void<
	decltype
	(
	 std::declval<FomMassMatrixOperatorType const>()
	 (
	   std::declval<typename TrialSubspaceType::full_state_type const &>(),
	   std::declval<typename TrialSubspaceType::basis_matrix_type const &>(),
	   std::declval<typename FomSystemType::time_type const &>(),
	   std::declval<
	     concepts::impl::fom_mass_matrix_action_on_trial_space_t<FomMassMatrixOperatorType, TrialSubspaceType > &
	   >()
	 )
	)
       >::value
    //
    && std::is_void<
	  decltype
	  (
	  ::pressio::ops::product
	    (::pressio::transpose(),
	     ::pressio::nontranspose(),
	     std::declval<scalar_trait_t<typename TrialSubspaceType::basis_matrix_type> const &>(),
	     std::declval<typename TrialSubspaceType::basis_matrix_type const &>(),
	     std::declval<
	       concepts::impl::fom_mass_matrix_action_on_trial_space_t<FomMassMatrixOperatorType, TrialSubspaceType>
	     const &>(),
	     std::declval<scalar_trait_t<typename TrialSubspaceType::reduced_state_type> const &>(),
	     std::declval<impl::explicit_galerkin_default_reduced_mass_matrix_t<TrialSubspaceType> &>()
	   )
	   )
      >::value
   >
  > : std::true_type{};

}}}} //end namespace pressio::rom::galerkin::unsteadyexplicit

#endif

#endif  // ROM_CONCEPTS_GALERKIN_EXPLICIT_DEFAULT_HPP_

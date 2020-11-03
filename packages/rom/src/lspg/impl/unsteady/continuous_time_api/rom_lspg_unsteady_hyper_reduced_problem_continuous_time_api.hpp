/*
//@HEADER
// ************************************************************************
//
// rom_lspg_unsteady_hyper_reduced_problem_continuous_time_api.hpp
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

#ifndef ROM_LSPG_IMPL_UNSTEADY_CONTINUOUS_TIME_API_ROM_LSPG_UNSTEADY_HYPER_REDUCED_PROBLEM_CONTINUOUS_TIME_API_HPP_
#define ROM_LSPG_IMPL_UNSTEADY_CONTINUOUS_TIME_API_ROM_LSPG_UNSTEADY_HYPER_REDUCED_PROBLEM_CONTINUOUS_TIME_API_HPP_


namespace pressio{ namespace rom{ namespace lspg{ namespace impl{ namespace unsteady{

template <typename ...Args>
class HyperReducedProblemContinuousTimeApi
{
public:
  using this_t = HyperReducedProblemContinuousTimeApi<Args...>;
  using traits = ::pressio::rom::details::traits<this_t>;

  using fom_system_t		= typename traits::fom_system_t;
  using fom_native_state_t	= typename traits::fom_native_state_t;
  using fom_state_t		= typename traits::fom_state_t;
  using lspg_state_t		= typename traits::lspg_state_t;
  using lspg_native_state_t	= typename traits::lspg_native_state_t;
  using decoder_t		= typename traits::decoder_t;
  using fom_state_reconstr_t	= typename traits::fom_state_reconstr_t;
  using fom_states_manager_t	= typename traits::fom_states_manager_t;
  using residual_policy_t	= typename traits::residual_policy_t;
  using jacobian_policy_t	= typename traits::jacobian_policy_t;
  using aux_stepper_t		= typename traits::aux_stepper_t;
  using stepper_t		= typename traits::stepper_t;
  static constexpr auto binding_sentinel = traits::binding_sentinel;

  using sample_to_stencil_t        = typename traits::sample_to_stencil_t;
  using sample_to_stencil_native_t = typename traits::sample_to_stencil_native_t;

private:
  using At    = FomObjMixin<fom_system_t, binding_sentinel>;
  using Bt    = FomStatesMngrMixin<At, void, fom_state_t,
				   fom_state_reconstr_t, fom_states_manager_t>;
  using Ct    = HypRedPoliciesMixin<Bt, void, residual_policy_t,
				    jacobian_policy_t, sample_to_stencil_t>;
  using mem_t = StepperMixin<Ct, aux_stepper_t, stepper_t>;
  mem_t members_;

public:
  stepper_t & stepperRef(){ return members_.stepperObj_; }

  const fom_native_state_t & currentFomStateCRef() const{
    return *(members_.fomStatesMngr_.currentFomStateCRef().data());
  }

  const fom_state_reconstr_t & fomStateReconstructorCRef() const{
    return members_.fomStateReconstructor_;
  }

public:
  HyperReducedProblemContinuousTimeApi() = delete;
  HyperReducedProblemContinuousTimeApi(const HyperReducedProblemContinuousTimeApi &) = default;
  HyperReducedProblemContinuousTimeApi & operator=(const HyperReducedProblemContinuousTimeApi &) = delete;
  HyperReducedProblemContinuousTimeApi(HyperReducedProblemContinuousTimeApi &&) = default;
  HyperReducedProblemContinuousTimeApi & operator=(HyperReducedProblemContinuousTimeApi &&) = delete;
  ~HyperReducedProblemContinuousTimeApi() = default;

  template<
    typename _sample_to_stencil_t = sample_to_stencil_t,
    ::pressio::mpl::enable_if_t<std::is_void<_sample_to_stencil_t>::value, int > = 0
    >
  HyperReducedProblemContinuousTimeApi(const fom_system_t & fomObj,
				       const decoder_t & decoder,
				       const lspg_state_t & romStateIn,
				       const fom_native_state_t fomNominalStateIn)
    : members_(romStateIn, fomObj, decoder, fomNominalStateIn)
  {}

  template <
    bool _binding_sentinel = binding_sentinel,
    ::pressio::mpl::enable_if_t<!_binding_sentinel, int > = 0
    >
  HyperReducedProblemContinuousTimeApi(const fom_system_t & fomObj,
				       const decoder_t & decoder,
				       const lspg_state_t & romStateIn,
				       const fom_native_state_t fomNominalStateIn,
				       const sample_to_stencil_t & sTosInfo)
    : members_(romStateIn, fomObj, decoder, fomNominalStateIn, sTosInfo)
  {}


#ifdef PRESSIO_ENABLE_TPL_PYBIND11
  template <
    bool _binding_sentinel = binding_sentinel,
    ::pressio::mpl::enable_if_t<_binding_sentinel, int > = 0
    >
  HyperReducedProblemContinuousTimeApi(const fom_system_t & fomObjPy,
				       const decoder_t & decoder,
				       const lspg_native_state_t & romStateIn,
				       const fom_native_state_t fomNominalStateIn,
				       sample_to_stencil_native_t sTosInfo)
    : members_(lspg_state_t(romStateIn), fomObjPy, decoder,
	       fomNominalStateIn, sTosInfo)
  {}
#endif

};

}}}}}
#endif  // ROM_LSPG_IMPL_UNSTEADY_CONTINUOUS_TIME_API_ROM_LSPG_UNSTEADY_HYPER_REDUCED_PROBLEM_CONTINUOUS_TIME_API_HPP_

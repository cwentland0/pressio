/*
//@HEADER
// ************************************************************************
//
// rom_lspg_unsteady_cont_time_decorators.hpp
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

#ifndef ROM_IMPL_LSPG_UNSTEADY_PRECONDITIONING_DECORATOR_HPP_
#define ROM_IMPL_LSPG_UNSTEADY_PRECONDITIONING_DECORATOR_HPP_

namespace pressio{ namespace rom{ namespace impl{

template <
  class PreconditionerType,
  class ResidualType,
  class JacobianType,
  class TrialSubspaceType,
  class Preconditionable
  >
class LspgPreconditioningDecorator : public Preconditionable
{
public:
  // required
  using independent_variable_type = typename Preconditionable::independent_variable_type;
  using state_type    = typename Preconditionable::state_type;
  using residual_type = typename Preconditionable::residual_type;
  using jacobian_type = typename Preconditionable::jacobian_type;

  template <
    class FomSystemType,
    template<class> class LspgFomStatesManager
    >
  LspgPreconditioningDecorator(const TrialSubspaceType & trialSubspace,
			       const FomSystemType & fomSystem,
			       LspgFomStatesManager<TrialSubspaceType> & fomStatesManager,
			       const PreconditionerType & preconditioner)
    : Preconditionable(trialSubspace, fomSystem, fomStatesManager),
      fomStatesManager_(fomStatesManager),
      preconditioner_(preconditioner)
  {}

public:
  template <class StencilStatesContainerType, class StencilRhsContainerType>
  void operator()(::pressio::ode::StepScheme odeSchemeName,
		  const state_type & predictedReducedState,
		  const StencilStatesContainerType & reducedStatesStencilManager,
		  StencilRhsContainerType & fomRhsStencilManger,
		  const ::pressio::ode::StepEndAt<independent_variable_type> & rhsEvaluationTime,
		  ::pressio::ode::StepCount step,
		  const ::pressio::ode::StepSize<independent_variable_type> & dt,
		  residual_type & R,
		  jacobian_type & J,
		  bool computeJacobian) const
  {
    Preconditionable::operator()(odeSchemeName, predictedReducedState,
				 reducedStatesStencilManager, fomRhsStencilManger,
				 rhsEvaluationTime, step, dt, R, J,
				 computeJacobian);
    // fomStateAt_np1 is only valid IF already reconstructed in the base class
    const auto & fomStateAt_np1 = fomStatesManager_(::pressio::ode::nPlusOne());
    preconditioner_(fomStateAt_np1, rhsEvaluationTime.get(), R, J);
  }

private:
  std::reference_wrapper<LspgFomStatesManager<TrialSubspaceType>> fomStatesManager_;
  std::reference_wrapper<const PreconditionerType> preconditioner_;
};

}}}
#endif  // ROM_IMPL_LSPG_UNSTEADY_MASK_DECORATOR_HPP_

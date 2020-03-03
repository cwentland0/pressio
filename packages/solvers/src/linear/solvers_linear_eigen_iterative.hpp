/*
//@HEADER
// ************************************************************************
//
// solvers_linear_eigen_iterative.hpp
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

#ifndef SOLVERS_LINEAR_EIGEN_ITERATIVE_HPP
#define SOLVERS_LINEAR_EIGEN_ITERATIVE_HPP

namespace pressio { namespace solvers { namespace iterative{

template<typename solver_tag, typename MatrixT>
class EigenIterative
  : public LinearBase<MatrixT, EigenIterative<solver_tag, MatrixT> >,
    public IterativeBase< EigenIterative<solver_tag, MatrixT>,
			  typename containers::details::traits<MatrixT>::scalar_t>
{
public:

  static_assert( ::pressio::containers::meta::is_matrix_wrapper_eigen<MatrixT>::value or
  		 ::pressio::containers::meta::is_multi_vector_wrapper_eigen<MatrixT>::value,
  		 "Eigen iterative solver needs a matrix type = wrapper of an eigen matrix");

  using solver_t	= solver_tag;
  using matrix_type	= MatrixT;
  using native_mat_t    = typename containers::details::traits<MatrixT>::wrapped_t;
  using scalar_t        = typename containers::details::traits<MatrixT>::scalar_t;
  using this_t          = EigenIterative<solver_tag, MatrixT>;
  using base_interface  = LinearBase<MatrixT, this_t>;
  using base_iterative  = IterativeBase<this_t, scalar_t>;
  using solver_traits   = linear::details::traits<solver_tag>;
  using native_solver_t = typename solver_traits::template eigen_solver_type<native_mat_t>;
  using iteration_t	= typename base_iterative::iteration_t;

  static_assert( solver_traits::eigen_enabled == true,
		 "the native solver must be from Eigen to use in EigenIterative");

  static_assert( solver_traits::direct == false,
		 "The native eigen solver must be iterative to use in EigenIterative");

public:
  EigenIterative() = default;
  EigenIterative(const EigenIterative &) = delete;
  ~EigenIterative() = default;

private:

  iteration_t getNumIterationsExecutedImpl() const {
    return mysolver_.iterations();
  }

  scalar_t getFinalErrorImpl() const {
    return mysolver_.error();
  }

  void resetLinearSystemImpl(const MatrixT& A) {
    mysolver_.compute(*A.data());
  }

  template <typename T>
  void solveImpl(const T& b, T & y)
  {
    *y.data() = mysolver_.solve(*b.data());
  }

  template <typename T>
  void solveImpl(const MatrixT & A, const T& b, T & y) {
    this->resetLinearSystem(A);
    this->solve(b, y);
  }

  template <typename T>
  void solveAllowMatOverwriteImpl(MatrixT & A, const T& b, T & y) {
    this->resetLinearSystem(A);
    this->solve(b, y);
  }

  friend base_iterative;
  friend base_interface;
  native_solver_t mysolver_ = {};
};

}}} // end namespace pressio::solvers::iterative
#endif

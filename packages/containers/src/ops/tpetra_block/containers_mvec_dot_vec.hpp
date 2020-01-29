/*
//@HEADER
// ************************************************************************
//
// containers_mvec_dot_vec.hpp
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

#ifdef PRESSIO_ENABLE_TPL_TRILINOS
#ifndef CONTAINERS_SRC_OPS_TPETRA_BLOCK_MULTI_VECTOR_DOT_VECTOR_HPP_
#define CONTAINERS_SRC_OPS_TPETRA_BLOCK_MULTI_VECTOR_DOT_VECTOR_HPP_

#include "../../multi_vector/containers_multi_vector_meta.hpp"

namespace pressio{ namespace containers{ namespace ops{

/*
 * c = A dot b
 * where:
 * A = wrapper of Tpetra block Multivector
 * b = wrapper of tpetra block vector
 * c = a shared-mem vector, like eigen or armadillo or std::vector or kokkos::view
 */

//------------------------------------
// c = wrapper of Kokkos vector
//------------------------------------
template <
  typename mvec_type,
  typename vec_type,
  typename result_type,
  ::pressio::mpl::enable_if_t<
    containers::meta::is_multi_vector_wrapper_tpetra_block<mvec_type>::value &&
    containers::meta::is_vector_wrapper_tpetra_block<vec_type>::value &&
    containers::meta::is_vector_wrapper_kokkos<result_type>::value
    > * = nullptr
  >
void dot(const mvec_type & mvA, const vec_type & vecB, result_type & result)
{
  static_assert(containers::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value and
		containers::meta::wrapper_pair_have_same_scalar<result_type, vec_type>::value,
		"Tpetra MV dot V: operands do not have matching scalar type");

  static_assert(std::is_same<
		typename containers::details::traits<mvec_type>::device_t,
		typename containers::details::traits<vec_type>::device_t>::value,
		"Tpetra MV dot V: operands do not have the same device type");

  static_assert(std::is_same<
		typename containers::details::traits<vec_type>::device_t,
		typename containers::details::traits<result_type>::device_t>::value,
		"Tpetra MV dot V: V and result do not have the same device type");

  const auto mvA_mvv = mvA.data()->getMultiVectorView();
  using tpetra_blockvector_t = typename containers::details::traits<vec_type>::wrapped_t;
  const auto vecB_vv = const_cast<tpetra_blockvector_t*>(vecB.data())->getVectorView();
  auto request = Tpetra::idot( *result.data(), mvA_mvv, vecB_vv);
  request->wait();
}

//------------------------------------
// c = scalar *, passed in
//------------------------------------
template <
  typename mvec_type,
  typename vec_type,
  ::pressio::mpl::enable_if_t<
    containers::meta::is_multi_vector_wrapper_tpetra_block<mvec_type>::value &&
    containers::meta::is_vector_wrapper_tpetra_block<vec_type>::value &&
    containers::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value
    > * = nullptr
  >
void dot(const mvec_type & mvA, const vec_type & vecB,
	 typename details::traits<mvec_type>::scalar_t * result){

  /* workaround the non-constness of getVectorView*/
  using tpetra_blockvec_t = typename containers::details::traits<vec_type>::wrapped_t;
  const auto vecB_vv = const_cast<tpetra_blockvec_t*>(vecB.data())->getVectorView();

  const auto mvA_mvv = mvA.data()->getMultiVectorView();
  const auto numVecs = mvA.globalNumVectors();
  for (std::size_t i=0; i<(std::size_t)numVecs; i++){
    // colI is a Teuchos::RCP<Vector<...>>
    const auto colI = mvA_mvv.getVector(i);
    result[i] = colI->dot(vecB_vv);
  }
}


//--------------------------------------------
// c = teuchos serial dense vector, passed in
//--------------------------------------------
template <
  typename mvec_type,
  typename vec_type,
  typename result_vec_type,
  ::pressio::mpl::enable_if_t<
    containers::meta::is_multi_vector_wrapper_tpetra_block<mvec_type>::value and
    containers::meta::is_vector_wrapper_tpetra_block<vec_type>::value and
    containers::meta::is_dense_vector_wrapper_teuchos<result_vec_type>::value and
    containers::meta::wrapper_triplet_have_same_scalar<mvec_type,
						       vec_type,
						       result_vec_type>::value and
    containers::details::traits<result_vec_type>::is_dynamic
    > * = nullptr
  >
void dot(const mvec_type & mvA, const vec_type & vecB, result_vec_type & result)
{
  const auto numVecs = mvA.globalNumVectors();
  if ( result.size() != numVecs )
    result.resize(numVecs);
  dot(mvA, vecB, result.data()->values());
}


//--------------------------------------
// c = Eigen DYNAMIC vector, passed in
//--------------------------------------
template <
  typename mvec_type,
  typename vec_type,
  typename result_vec_type,
  ::pressio::mpl::enable_if_t<
    containers::meta::is_multi_vector_wrapper_tpetra_block<mvec_type>::value and
    containers::meta::is_vector_wrapper_tpetra_block<vec_type>::value and
    containers::meta::is_vector_wrapper_eigen<result_vec_type>::value and
    containers::meta::wrapper_triplet_have_same_scalar<mvec_type,
						       vec_type,
						       result_vec_type>::value and
    containers::details::traits<result_vec_type>::is_dynamic
    > * = nullptr
  >
void dot(const mvec_type & mvA, const vec_type & vecB, result_vec_type & result){

  ///computes dot product of each vector in mvA
  ///with vecB storing each value in result

  /* Apparently, trilinos does not support this...
     the native dot() method of multivectors is only for
     dot product of two multivectors with same # of columns.
     So we have to extract each column vector
     from mvA and do dot product one a time*/

  const auto numVecs = mvA.globalNumVectors();
  // check the result has right size
  if ( result.size() != numVecs )
    result.resize(numVecs);

  ::pressio::containers::ops::set_zero(result);
  dot(mvA, vecB, result.data()->data());
}


//--------------------------------------
// c = Eigen STATIC vector, passed in
//--------------------------------------
template <
  typename mvec_type,
  typename vec_type,
  typename result_vec_type,
  ::pressio::mpl::enable_if_t<
    containers::meta::is_multi_vector_wrapper_tpetra_block<mvec_type>::value and
    containers::meta::is_vector_wrapper_tpetra_block<vec_type>::value and
    containers::meta::is_vector_wrapper_eigen<result_vec_type>::value and
    containers::meta::wrapper_triplet_have_same_scalar<mvec_type, vec_type, result_vec_type>::value and
    containers::details::traits<result_vec_type>::is_static
    > * = nullptr
  >
void dot(const mvec_type & mvA, const vec_type & vecB, result_vec_type & result)
{
  ///computes dot product of each vector in mvA
  ///with vecB storing each value in result
  // check the result has right size
  assert( result.size() == mvA.globalNumVectors() );
  dot(mvA, vecB, result.data()->data());
}

}}}//end namespace pressio::containers::ops
#endif
#endif

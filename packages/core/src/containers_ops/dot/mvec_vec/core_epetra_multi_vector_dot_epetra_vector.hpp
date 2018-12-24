
#ifdef HAVE_TRILINOS
#ifndef CORE_EPETRA_MULTI_VECTOR_DOT_EPETRA_VECTOR_HPP_
#define CORE_EPETRA_MULTI_VECTOR_DOT_EPETRA_VECTOR_HPP_

#include "../../core_ops_meta.hpp"
#include "../../../vector/core_vector_meta.hpp"
#include "../../../multi_vector/core_multi_vector_meta.hpp"

namespace rompp{ namespace core{ namespace ops{

/*
 * overloads for c = A dot b
 * where:
 * A = wrapper of Epetra Multivector
 * b = epetra vector
 * c = a shared-mem vector, like eigen or armadillo
 */


//------------------------------------
// c = scalar *, passed in
//------------------------------------
template <typename mvec_type,
	  typename vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value &&
    core::meta::is_epetra_vector_wrapper<vec_type>::value &&
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 typename details::traits<mvec_type>::scalar_t * result){

  ///computes dot product of each vector in mvA
  ///with vecB storing each value in result
  /* Apparently, trilinos does not support this...
   * the native Dot() method of multivectors is only for
   * dot product of two multivectors of the same size.
   * So we have to extract each column vector
   * from mvA and do dot product one at a time
  */

  // how many vectors are in mvA
  auto numVecs = mvA.globalNumVectors();
  // if ( result.size() != (size_t)numVecs )
  //   result.resize(numVecs);
  auto * mvNatData = mvA.data();
  const auto * vecNatData = vecB.data();
  for (decltype(numVecs) i=0; i<numVecs; i++){
    (*mvNatData)(i)->Dot(*vecNatData, &result[i]);
  }
}


//--------------------------------------------
// c = teuchos serial dense vector, passed in
//--------------------------------------------
template <typename mvec_type,
	  typename vec_type,
	  typename result_vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_epetra_vector_wrapper<vec_type>::value and
    core::meta::is_teuchos_serial_dense_vector_wrapper<result_vec_type>::value and
    core::meta::wrapper_triplet_have_same_scalar<mvec_type,
						 vec_type,
						 result_vec_type>::value and
    core::details::traits<result_vec_type>::is_dynamic
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 result_vec_type & result){
  auto numVecs = mvA.globalNumVectors();
  if ( result.size() != numVecs )
    result.resize(numVecs);
  dot(mvA, vecB, result.data()->values());
}


//--------------------------------------
// c = Eigen DYNAMIC vector, passed in
//--------------------------------------
template <typename mvec_type,
	  typename vec_type,
	  typename result_vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_epetra_vector_wrapper<vec_type>::value and
    core::meta::is_eigen_vector_wrapper<result_vec_type>::value and
    core::meta::wrapper_triplet_have_same_scalar<mvec_type,
						 vec_type,
						 result_vec_type>::value and
    core::details::traits<result_vec_type>::is_dynamic
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 result_vec_type & result){
  // how many vectors are in mvA
  auto numVecs = mvA.globalNumVectors();
  if ( result.size() != numVecs )
    result.resize(numVecs);
  dot(mvA, vecB, result.data()->data());
}


//--------------------------------------
// c = Eigen STATIC vector, passed in
//--------------------------------------
template <typename mvec_type,
	  typename vec_type,
	  typename result_vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_epetra_vector_wrapper<vec_type>::value and
    core::meta::is_eigen_vector_wrapper<result_vec_type>::value and
    core::meta::wrapper_triplet_have_same_scalar<mvec_type,
						 vec_type,
						 result_vec_type>::value and
    core::details::traits<result_vec_type>::is_static
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 result_vec_type & result){
  // how many vectors are in mvA
  auto numVecs = mvA.globalNumVectors();
  assert(result.size() == numVecs);
  dot(mvA, vecB, result.data()->data());
}


//--------------------------------------
// c = std::vector, passed in
//--------------------------------------
template <typename mvec_type,
	  typename vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value &&
    core::meta::is_epetra_vector_wrapper<vec_type>::value &&
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 std::vector<typename
	 details::traits<mvec_type>::scalar_t> & result){
  // how many vectors are in mvA
  auto numVecs = mvA.globalNumVectors();
  if ( result.size() != (size_t)numVecs )
    result.resize(numVecs);
  dot(mvA, vecB, result.data());
}


//--------------------------------------
// returns a std::vector
//--------------------------------------
template <typename mvec_type,
	  typename vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value &&
    core::meta::is_epetra_vector_wrapper<vec_type>::value &&
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value
    > * = nullptr
  >
std::vector<typename details::traits<mvec_type>::scalar_t>
dot(const mvec_type & mvA, const vec_type & vecB){

  using sc_t = typename details::traits<mvec_type>::scalar_t;
  // how many vectors are in mvA
  auto numVecs = mvA.globalNumVectors();
  using res_t = std::vector<sc_t>;
  res_t res(numVecs);
  dot(mvA, vecB, res);
  return res;
}
//--------------------------------------------------------

}}} // end namespace rompp::core::ops
#endif
#endif

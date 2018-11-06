
#ifndef CORE_MULTI_VECTOR_VECTOR_DOT_PRODUCT_HPP_
#define CORE_MULTI_VECTOR_VECTOR_DOT_PRODUCT_HPP_

#include "../core_ops_meta.hpp"
#include "../../vector/core_vector_meta.hpp"
#include "../../multi_vector/core_multi_vector_meta.hpp"

namespace rompp{ namespace core{ namespace ops{

// Eigen multivector dot eigen vector
// result stored in Eigen vector passed by reference 
template <typename mvec_type,
	  typename vec_type,
	  typename result_vec_type,
  core::meta::enable_if_t<
    core::meta::is_eigen_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_eigen_vector_wrapper<vec_type>::value and 
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value and 
    core::meta::is_eigen_vector_wrapper<result_vec_type>::value and
    core::meta::wrapper_pair_have_same_scalar<vec_type,result_vec_type>::value
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 result_vec_type & result){
  auto numVecs = mvA.numVectors();
  if ( result.size() != numVecs )
    result.resize(numVecs);
  *result.data() = (*mvA.data()).transpose() * (*vecB.data());
}
//--------------------------------------------------------

      
// Eigen multivector dot eigen vector
// result is built and returned 
template <typename mvec_type,
	  typename vec_type,
  core::meta::enable_if_t<
    core::meta::is_eigen_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_eigen_vector_wrapper<vec_type>::value and 
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value
    > * = nullptr
  >
auto dot(const mvec_type & mvA,
	 const vec_type & vecB){
  using sc_t = typename core::details::traits<mvec_type>::scalar_t;
  core::Vector<Eigen::Matrix<sc_t, Eigen::Dynamic, 1>> c(vecB.size());
  dot(mvA,vecB,c);
  return c;
}
//--------------------------------------------------------


#ifdef HAVE_TRILINOS    
// Epetra multivector dot epetra vector
// result is a * passed as an argument to change
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
     the native Dot() method of multivectors is only for 
     dot product of two multivectors of the same size. 
     So we have to extract each column vector 
     from mvA and do dot product one a time*/
  
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
#endif
//--------------------------------------------------------

#ifdef HAVE_TRILINOS    
// Epetra multivector dot epetra vector
// result is an Eigen vector passed by reference to store the solution
template <typename mvec_type,
	  typename vec_type,
	  typename result_vec_type,
  core::meta::enable_if_t<
    core::meta::is_epetra_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_epetra_vector_wrapper<vec_type>::value and 
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value and 
    core::meta::is_eigen_vector_wrapper<result_vec_type>::value and
    core::meta::wrapper_pair_have_same_scalar<vec_type,result_vec_type>::value
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
#endif
//--------------------------------------------------------
      
      
#ifdef HAVE_TRILINOS    
// Epetra multivector dot epetra vector
// result is a std::vector passed by reference 
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
#endif
//--------------------------------------------------------

      
#ifdef HAVE_TRILINOS  
//  Epetra multivector dot epetra vector
// this creates and returns a std::vector holding the result
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
#endif
//--------------------------------------------------------


#ifdef HAVE_TRILINOS    
// Tpetra multivector dot tpetra vector
// result stored into Eigen vector passed by reference
template <typename mvec_type,
	  typename vec_type,
	  typename result_vec_type,
  core::meta::enable_if_t<
    core::meta::is_tpetra_multi_vector_wrapper<mvec_type>::value and
    core::meta::is_tpetra_vector_wrapper<vec_type>::value and 
    core::meta::wrapper_pair_have_same_scalar<mvec_type, vec_type>::value and 
    core::meta::is_eigen_vector_wrapper<result_vec_type>::value and
    core::meta::wrapper_pair_have_same_scalar<vec_type,result_vec_type>::value
    > * = nullptr
  >
void dot(const mvec_type & mvA,
	 const vec_type & vecB,
	 result_vec_type & result){

  ///computes dot product of each vector in mvA
  ///with vecB storing each value in result
  
  /* Apparently, trilinos does not support this... 
     the native dot() method of multivectors is only for 
     dot product of two multivectors with same # of columns.
     So we have to extract each column vector 
     from mvA and do dot product one a time*/

  // how many vectors are in mvA
  auto numVecs = mvA.globalNumVectors();
  // check the result has right size
  if ( result.size() != numVecs )
    result.resize(numVecs);
  
  for (decltype(numVecs) i=0; i<numVecs; i++){
    // colI is a Teuchos::RCP<Vector<...>>
    auto colI = mvA.data()->getVector(i); 
    result[i] = colI->dot(*vecB.data());
  }
}
#endif
//--------------------------------------------------------
      
  
}}} // end namespace rompp::core::ops
#endif

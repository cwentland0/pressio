
#ifndef CORE_NORM2_VECTOR_HPP_
#define CORE_NORM2_VECTOR_HPP_

#include "../core_ops_meta.hpp"
#include "../../vector/core_vector_meta.hpp"

namespace rompp{ namespace core{ namespace ops{
  
//  eigen vector wrapper 
template <typename vec_type,
  core::meta::enable_if_t<
    core::meta::is_eigen_vector_wrapper<vec_type>::value
    > * = nullptr
  >
auto norm2(const vec_type & a)
  -> typename details::traits<vec_type>::scalar_t
{
  using sc_t = typename details::traits<vec_type>::scalar_t;
  sc_t result = 0.0;
  for (decltype(a.size()) i=0; i<a.size(); i++)
    result += a[i]*a[i];
  return std::sqrt(result);
}
//--------------------------------------------------------

  
}}}//end namespace rompp::core::ops
#endif

/*
//@HEADER
// ************************************************************************
//
// rom_has_static_method_axpy.hpp
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

#ifndef ROM_ROM_HAS_STATIC_METHOD_AXPY_HPP_
#define ROM_ROM_HAS_STATIC_METHOD_AXPY_HPP_

#include "../../../mpl/src/mpl_variadic.hpp"
#include "../../../mpl/src/mpl_non_variadic.hpp"

namespace pressio{ namespace rom{ namespace meta {

template <
  typename T,
  typename x_t, typename y_t, typename a_t,
  typename = void
  >
struct has_static_method_axpy
  : std::false_type{};

template <
  typename T,
  typename x_t, typename y_t, typename a_t
  >
struct has_static_method_axpy<
  T, x_t, y_t, a_t,
  mpl::enable_if_t<
    std::is_void<
      decltype
      (
       T::axpy
       (
	std::declval< a_t >(),
	std::declval< x_t const & >(),
	std::declval< y_t & >()
	)
       )
      >::value
    >
  > : std::true_type{};

}}} //pressio::rom::meta
#endif

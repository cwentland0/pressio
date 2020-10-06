/*
//@HEADER
// ************************************************************************
//
// containers_multi_vector_arbitrary.hpp
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

#ifndef CONTAINERS_MULTI_VECTOR_CONCRETE_CONTAINERS_MULTI_VECTOR_ARBITRARY_HPP_
#define CONTAINERS_MULTI_VECTOR_CONCRETE_CONTAINERS_MULTI_VECTOR_ARBITRARY_HPP_

namespace pressio{ namespace containers{

template <typename wrapped_type>
class MultiVector<
  wrapped_type,
  mpl::enable_if_t<
    ::pressio::containers::predicates::is_admissible_as_multi_vector_arbitrary<wrapped_type>::value
    >
  >
{
  using this_t = MultiVector<wrapped_type>;
  using size_t = typename details::traits<this_t>::size_t;
  using sc_t   = typename details::traits<this_t>::scalar_t;

public:
  template<
    typename _wrapped_type = wrapped_type,
    mpl::enable_if_t<
      std::is_default_constructible<_wrapped_type>::value, int
      > = 0
  >
  MultiVector(){};

  template<
    typename _wrapped_type = wrapped_type,
    mpl::enable_if_t<
      std::is_constructible<_wrapped_type, size_t, size_t>::value, int> = 0
  >
  MultiVector(size_t nR, size_t nC) : data_(nR, nC){};


  explicit MultiVector(const wrapped_type & vecobj)
    : data_(vecobj){}

  template<
    typename T = int,
    mpl::enable_if_t<std::is_move_constructible<wrapped_type>::value, T> = 0
  >
  explicit MultiVector(wrapped_type && vecobj)
    : data_(vecobj){}


  // copy constructor
  MultiVector(MultiVector const & other)
    : data_(*other.data()){}

  // copy assignment
  MultiVector & operator=(const MultiVector & other){
    if(&other != this){
      data_ = other.data_;
    }
    return *this;
  }

  // mv constructor
  template<
    typename T = int,
    mpl::enable_if_t<std::is_move_constructible<wrapped_type>::value, T> = 0
  >
  MultiVector(MultiVector && other) noexcept
    : data_(std::move(other.data_)){}

  // mv assignment
  template<
    typename T = int,
    mpl::enable_if_t<std::is_move_constructible<wrapped_type>::value, T> = 0
  >
  MultiVector & operator=(MultiVector && other) noexcept
  {
    if(&other != this){
      data_ = std::move(other.data_);
    }
    return *this;
  }

public:
  size_t extent(size_t k) const{
    return data_.extent(k);
  }

  sc_t & operator()(size_t i, size_t j){
    return data_(i, j);
  };
  sc_t const & operator()(size_t i, size_t j) const{
    return data_(i, j);
  };

public:
  wrapped_type const * data() const{
    return &data_;
  }

  wrapped_type * data(){
    return &data_;
  }

private:
  friend ContainerBase<this_t>;
  wrapped_type data_ = {};

};//end class

}}//end namespace pressio::containers

#endif  // CONTAINERS_MULTI_VECTOR_CONCRETE_CONTAINERS_MULTI_VECTOR_ARBITRARY_HPP_

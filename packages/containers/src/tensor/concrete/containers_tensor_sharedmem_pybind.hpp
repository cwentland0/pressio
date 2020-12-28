/*
//@HEADER
// ************************************************************************
//
// containers_static_collection_impl.hpp
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

#ifndef CONTAINERS_COLLECTION_CONTAINERS_TENSOR_HPP_
#define CONTAINERS_COLLECTION_CONTAINERS_TENSOR_HPP_

namespace pressio{ namespace containers{

template <int rankIn, typename wrapped_type>
class Tensor<
  rankIn, wrapped_type,
  ::pressio::mpl::enable_if_t<
    containers::predicates::is_array_pybind<wrapped_type>::value
    >
  >
{
  static_assert
  (rankIn <=3, "Pressio tensor wrapper currently only supports rank<=3");

  using this_t	    = Tensor<rankIn, wrapped_type>;
  using mytraits    = typename details::traits<this_t>;
  using sc_t	    = typename mytraits::scalar_t;
  using ord_t	    = typename mytraits::ordinal_t;
  using wrap_t	    = typename mytraits::wrapped_t;
  using ref_t	    = typename mytraits::reference_t;
  using const_ref_t = typename mytraits::const_reference_t;
  using mut_proxy_t = typename mytraits::mut_proxy_t;
  using proxy_t	    = typename mytraits::proxy_t;

public:
  static constexpr int rank = rankIn;

  Tensor() = delete;

  // -----------------------------
  // constructor with extents
  // -----------------------------
  // rank-1
  template<int _rank = rank, mpl::enable_if_t<_rank==1, int> = 0>
  explicit Tensor(std::size_t ext) : data_(ext1){}

  // rank-2
  template<int _rank = rank, mpl::enable_if_t<_rank==2, int> = 0>
  Tensor(std::size_t ext1, std::size_t ext2)
    : data_({ext1, ext2})
  {}

  // rank-2
  template<int _rank = rank, mpl::enable_if_t<_rank==3, int> = 0>
  Tensor(std::size_t ext1, std::size_t ext2, std::size_t ext3)
    : data_({ext1, ext2, ext3})
  {}

  // -----------------------------
  // constructor from pybind::array
  // -----------------------------
  explicit Tensor(const wrap_t & src)
    : data_{ wrap_t(const_cast<wrap_t &>(src).request()) }
  {
    if ( rank==1 and data_.ndim() != 1 ){
      throw std::runtime_error
	("You cannot construct a rank-1 tensor wrapper from a pybind::array with ndim!=1");
    }
    else if ( rank==2 and data_.ndim() != 2 ){
      throw std::runtime_error
	("You cannot construct a rank-2 tensor wrapper from a pybind::array with ndim!=2");
    }
    else if ( rank==3 and data_.ndim() != 3 ){
      throw std::runtime_error
	("You cannot construct a rank-3 tensor wrapper from a pybind::array with ndim!=3");
    }
    else{
      throw std::runtime_error("Invalid case");
    }

    const auto srcPx = src.unchecked();
    copySourceProxyToMineRank(srcPx);
  }

  // -----------------------------
  // constructor with view semantics
  // use only this if you know what you are doing
  // -----------------------------
  Tensor(wrap_t src, ::pressio::view) : data_{src}
  {
    if ( rank==1 and data_.ndim() != 1 ){
      throw std::runtime_error
	("You cannot construct a rank-1 tensor wrapper from a pybind::array with ndim!=1");
    }
    else if ( rank==2 and data_.ndim() != 2 ){
      throw std::runtime_error
	("You cannot construct a rank-2 tensor wrapper from a pybind::array with ndim!=2");
    }
    else if ( rank==3 and data_.ndim() != 3 ){
      throw std::runtime_error
	("You cannot construct a rank-3 tensor wrapper from a pybind::array with ndim!=3");
    }
    else{
      throw std::runtime_error("Invalid case");
    }
  }

  // -----------------------------
  // copy constructor
  // -----------------------------
  // copy cnstr rank-1
  template<int _rank = rank, mpl::enable_if_t<_rank==1, int> = 0>
  Tensor(Tensor const & other) : data_({ other.extent(0) })
  {
    const auto srcPx = other.data_.unchecked();
    copySourceProxyToMine(srcPx);
  }

  // copy cnstr rank-2
  template<int _rank = rank, mpl::enable_if_t<_rank==2, int> = 0>
  Tensor(Tensor const & other) : data_({ other.extent(0), other.extent(1) })
  {
    const auto srcPx = other.data_.unchecked();
    copySourceProxyToMine(srcPx);
  }

  // copy cnstr rank-3
  template<int _rank = rank, mpl::enable_if_t<_rank==3, int> = 0>
  Tensor(Tensor const & other) : data_({ other.extent(0), other.extent(1), other.extent(2) })
  {
    const auto srcPx = other.data_.unchecked();
    copySourceProxyToMine(srcPx);
  }

  // copy assignment
  Tensor & operator=(const Tensor & other) = delete;
  Tensor(Tensor && other) = default;
  Tensor & operator=(Tensor && o) = delete;
  ~Tensor(){};

public:
  ord_t extent(ord_t i) const { return data_.shape(i); }

  wrap_t const * data() const{ return &data_; }

  wrap_t * data(){ return &data_; }

  proxy_t proxy() const{ return data_.unchecked(); }

  mut_proxy_t proxy(){ return data_.mutable_unchecked(); }

  // rank-1 subscripting
  template<int _rank = rank>
  mpl::enable_if_t<_rank==1, ref_t>
  operator()(ord_t i){ return data_(i); };

  template<int _rank = rank>
  mpl::enable_if_t<_rank==1, const_ref_t>
  operator()(ord_t i) const{ return data_(i); };

  // rank-2 subscripting
  template<int _rank = rank>
  mpl::enable_if_t<_rank==2, ref_t>
  operator()(ord_t i, ord_t j){ return data_(i,j); };

  template<int _rank = rank>
  mpl::enable_if_t<_rank==2, const_ref_t>
  operator()(ord_t i, ord_t j) const{ return data_(i,j); };

  // rank-3 subscripting
  template<int _rank = rank>
  mpl::enable_if_t<_rank==3, ref_t>
  operator()(ord_t i, ord_t j, ord_t k){ return data_(i,j,k); };

  template<int _rank = rank>
  mpl::enable_if_t<_rank==3, const_ref_t>
  operator()(ord_t i, ord_t j, ord_t k) const{ return data_(i,j, k); };

private:
  // these copy are best for column-major layout
  // should specialize for when wrapped_t is row major
  template<typename source_proxy_t, int _rank = rank>
  mpl::enable_if_t<_rank==1, int>
  copySourceProxyToMine(source_proxy_t srcPx)
  {
    auto myProxy = data_.mutable_unchecked();
    for (ord_t i=0; i<srcPx.shape(0); ++i){
      myProxy(i) = srcPx(i);
    }
  }

  template<typename source_proxy_t, int _rank = rank>
  mpl::enable_if_t<_rank==2, int>
  copySourceProxyToMine(source_proxy_t srcPx)
  {
    auto myProxy = data_.mutable_unchecked();
    for (ord_t j=0; j<srcPx.shape(1); ++j){
      for (ord_t i=0; i<srcPx.shape(0); ++i){
	myProxy(i,j) = srcPx(i,j);
      }
    }
  }

  template<typename source_proxy_t, int _rank = rank>
  mpl::enable_if_t<_rank==3, int>
  copySourceProxyToMine(source_proxy_t srcPx)
  {
    auto myProxy = data_.mutable_unchecked();
    for (ord_t k=0; k<srcPx.shape(2); ++k){
      for (ord_t j=0; j<srcPx.shape(1); ++j){
	for (ord_t i=0; i<srcPx.shape(0); ++i){
	  myProxy(i,j,k) = srcPx(i,j,k);
	}
      }
    }
  }

private:
  wrap_t data_ = {};
};

}} //end namespace pressio::containers
#endif  // CONTAINERS_COLLECTION_CONTAINERS_STATIC_COLLECTION_IMPL_HPP_

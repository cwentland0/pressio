/*
//@HEADER
// ************************************************************************
//
// containers_vector_distributed_epetra.hpp
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
#ifndef CONTAINERS_VECTOR_CONCRETE_VECTOR_DISTRIBUTED_EPETRA_HPP_
#define CONTAINERS_VECTOR_CONCRETE_VECTOR_DISTRIBUTED_EPETRA_HPP_

#include "../../shared_base/containers_container_base.hpp"
#include "../../shared_base/containers_container_distributed_mpi_base.hpp"
#include "../../shared_base/containers_container_distributed_trilinos_base.hpp"
#include "../../shared_base/containers_container_subscriptable_base.hpp"
#include "../../shared_base/containers_container_resizable_base.hpp"
#include "../base/containers_vector_distributed_base.hpp"

namespace pressio{ namespace containers{

template <typename wrapped_type>
class Vector<wrapped_type,
	     typename
	     std::enable_if<
	       meta::is_vector_epetra<
		 wrapped_type>::value
	       >::type
	     >
  : public ContainerBase< Vector<wrapped_type>, wrapped_type >,
    public VectorDistributedBase< Vector<wrapped_type> >,
    public ContainerDistributedMpiBase< Vector<wrapped_type>,
     typename details::traits<Vector<wrapped_type>>::communicator_t >,
    public ContainerDistributedTrilinosBase< Vector<wrapped_type>,
     typename details::traits<Vector<wrapped_type>>::data_map_t >,
    public ContainerResizableBase< Vector<wrapped_type>, 1>,
    public ContainerSubscriptable1DBase< Vector<wrapped_type>,
     typename details::traits<Vector<wrapped_type>>::scalar_t,
     typename details::traits<Vector<wrapped_type>>::local_ordinal_t>{

  using this_t = Vector<wrapped_type>;
  using sc_t = typename details::traits<this_t>::scalar_t;
  using LO_t = typename details::traits<this_t>::local_ordinal_t;
  using GO_t = typename details::traits<this_t>::global_ordinal_t;
  using der_t = this_t;
  using wrap_t = typename details::traits<this_t>::wrapped_t;
  using map_t = typename details::traits<this_t>::data_map_t;
  using mpicomm_t = typename details::traits<this_t>::communicator_t;

public:
  // default cnstr
  Vector() = delete;

  // cnstrs
  explicit Vector(const map_t & mapobj) : data_(mapobj){}
  explicit Vector(const wrap_t & vecobj) : data_(vecobj){}

  // copy cnstr
  Vector(Vector const & other) = default;
  // copy assignment
  Vector & operator=(Vector const & other) = default;
  // move cnstr
  Vector(Vector && other) = default;
  // move assignment
  Vector & operator=(Vector && other) = default;
  // destructor
  ~Vector() = default;

public:
  sc_t & operator [] (LO_t i){
    assert(i < this->localSize());
    return data_[i];
  };
  sc_t const & operator [] (LO_t i) const{
    assert(i < this->localSize());
    return data_[i];
  };

  sc_t & operator()(LO_t i){
    assert(i < this->localSize());
    return data_[i];
  };
  sc_t const & operator()(LO_t i) const{
    assert(i < this->localSize());
    return data_[i];
  };

  // compound assignment when type(b) = type(this)
  // this += b
  Vector & operator+=(const Vector & other) {
    this->data_.Update(1.0, *other.data(), 1.0 );
    return *this;
  }

  // compound assignment when type(b) = type(this)
  // this -= b
  Vector & operator-=(const Vector & other) {
    this->data_.Update(-1.0, *other.data(), 1.0 );
    return *this;
  }

  void print(std::string tag) const{
    ::pressio::utils::io::print_stdout(tag);
    data_.Print(std::cout);
  }

private:

  void matchLayoutWithImpl(const der_t & other){
    data_.ReplaceMap( other.getDataMap() );
  }

  mpicomm_t const & commCRefImpl() const{
    return data_.Comm();
  }

  wrap_t const * dataImpl() const{
    return &data_;
  }

  wrap_t * dataImpl(){
    return &data_;
  }

  bool isDistributedGloballyImpl() const{
    return data_.DistributedGlobal();
  }

  void putScalarImpl(sc_t value) {
    data_.PutScalar(value);
  }

  bool emptyImpl() const{
    return this->globalSize()==0 ? true : false;
  }

  GO_t globalSizeImpl() const {
    return data_.GlobalLength();
  }

  LO_t localSizeImpl() const {
    return data_.MyLength();
  }

  void replaceGlobalValuesImpl(GO_t numentries,
			       const GO_t * indices,
			       const sc_t * values){
    data_.ReplaceGlobalValues(numentries, values, indices);
  }

  map_t const & getDataMapImpl() const{
    return data_.Map();
  }

  void replaceDataMapImpl(const map_t & mapObj){
    data_.ReplaceMap(mapObj);
  }

private:
  friend ContainerBase< this_t, wrapped_type >;
  friend VectorDistributedBase< this_t >;
  friend ContainerDistributedMpiBase< this_t, mpicomm_t >;
  friend ContainerDistributedTrilinosBase< this_t, map_t >;
  friend ContainerResizableBase< this_t, 1>;
  friend ContainerSubscriptable1DBase< this_t, sc_t, LO_t>;

private:
  wrap_t data_ = {};

};//end class

}}//end namespace pressio::containers
#endif
#endif






  // template<typename op_t, typename T,
  // 	   ::pressio::mpl::enable_if_t<
  // 	     std::is_same<T,this_t>::value
  // 	     > * = nullptr
  // 	   >
  // void inPlaceOpImpl(sc_t a1, sc_t a2, const T & other){
  //   // this = a1*this op a2*other;
  //   for (LO_t i=0; i<this->localSize(); i++)
  //     data_[i] = op_t()( a1*data_[i], a2*other[i] );
  // }


  // template<typename op_t, typename T,
  // 	   ::pressio::mpl::enable_if_t<
  // 	     std::is_same<T,this_t>::value
  // 	     > * = nullptr
  // 	   >
  // void inPlaceOpImpl(sc_t a1, const T & x1,
  // 		     sc_t a2, const T & x2){
  //   // this = a1*x1 op a2*x2;
  //   assert(this->globalSizeImpl() == x1.globalSizeImpl());
  //   assert(this->globalSizeImpl() == x2.globalSizeImpl());

  //   for (LO_t i=0; i<this->localSize(); i++)
  //     data_[i] = op_t()( a1*x1[i], a2*x2[i] );
  // }

  // template<typename op0_t, typename T,
  // 	   typename op1_t, typename op2_t, typename op3_t,
  // 	   ::pressio::mpl::enable_if_t<
  // 	     std::is_same<T,this_t>::value &&
  // 	     std::is_same<op0_t, std::plus<sc_t>>::value &&
  // 	     std::is_same<op1_t, op0_t>::value &&
  // 	     std::is_same<op2_t, op1_t>::value &&
  // 	     std::is_same<op3_t, op1_t>::value
  // 	     > * = nullptr
  // 	   >
  // void inPlaceOpImpl(sc_t a0, sc_t a1, const T & x1,
  // 		     sc_t a2, const T & x2,
  // 		     sc_t a3, const T & x3,
  // 		     sc_t a4, const T & x4){
  //   // this = a0 * this + (a1*x1) + (a2*x2) + (a3*x3) + (a4*x4)
  //   assert(this->globalSizeImpl() == x1.globalSizeImpl());
  //   assert(this->globalSizeImpl() == x2.globalSizeImpl());
  //   assert(this->globalSizeImpl() == x3.globalSizeImpl());
  //   assert(this->globalSizeImpl() == x4.globalSizeImpl());

  //   for (LO_t i=0; i<this->localSize(); i++)
  //     data_[i] = a0*data_[i] + a1*x1[i] + a2*x2[i]
  // 	+ a3*x3[i] + a4*x4[i];
  // }

  // void minValueImpl(sc_t & result) const {
  //   data_.MinValue(&result);
  // }

  // void maxValueImpl(sc_t & result) const {
  //   data_.MaxValue(&result);
  // }

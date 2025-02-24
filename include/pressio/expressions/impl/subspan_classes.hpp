/*
//@HEADER
// ************************************************************************
//
// subspan_classes.hpp
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

#ifndef EXPRESSIONS_IMPL_SUBSPAN_CLASSES_HPP_
#define EXPRESSIONS_IMPL_SUBSPAN_CLASSES_HPP_

namespace pressio{ namespace expressions{ namespace impl{

#ifdef PRESSIO_ENABLE_TPL_EIGEN
template <typename MatrixType>
class SubspanExpr<
  MatrixType,
  std::enable_if_t<
    ::pressio::is_dense_matrix_eigen<MatrixType>::value
    >
  >
{

  using traits = SubSpanTraits<SubspanExpr<MatrixType>>;
  using native_expr_t = typename traits::native_expr_type;
  using reference_t   = typename traits::reference_type;
  using pair_t = std::pair<std::size_t, std::size_t>;

private:
  MatrixType * operand_;
  std::size_t rowStart_;
  std::size_t colStart_;
  std::size_t endRow_;
  std::size_t endCol_;
  std::size_t numRows_ = {};
  std::size_t numCols_ = {};
  native_expr_t nativeExprObj_;

public:
  SubspanExpr(MatrixType & matObjIn,
	      const pair_t rowRangeIn,
	      const pair_t colRangeIn)
    : operand_(&matObjIn),
      rowStart_(std::get<0>(rowRangeIn)),
      colStart_(std::get<0>(colRangeIn)),
      endRow_(std::get<1>(rowRangeIn)-1),
      endCol_(std::get<1>(colRangeIn)-1),
      numRows_(endRow_ - rowStart_ + 1),
      numCols_(endCol_ - colStart_ + 1),
      nativeExprObj_(operand_->block(rowStart_, colStart_, numRows_, numCols_))
  {
    assert( rowStart_ >= 0 and rowStart_ < std::size_t(matObjIn.rows()) );
    assert( std::get<1>(rowRangeIn) <= std::size_t(matObjIn.rows()) );
    assert( colStart_ >= 0 and colStart_ < std::size_t(matObjIn.cols()) );
    assert( std::get<1>(colRangeIn) <= std::size_t(matObjIn.cols()) );

    // here the ranges are exclusive of the last index (like Kokkos and Python)
    // so the indices of the last row and col included are:
    assert(endRow_ >= rowStart_);
    assert(endCol_ >= colStart_);
  }

public:
  auto data() const { return operand_; }

  std::size_t extent(std::size_t i) const{
    if (i == 0) {
      return numRows_;
    } else if (i == 1) {
      return numCols_;
    } else {
      return std::size_t(1);
    }
  }

  native_expr_t const & native() const{ return nativeExprObj_; }
  native_expr_t & native(){ return nativeExprObj_; }

  reference_t operator()(const std::size_t & i, const std::size_t & j){
    assert(i < numRows_);
    assert(j < numCols_);
    return (*operand_)(rowStart_+i, colStart_+j);
  }

  reference_t operator()(const std::size_t & i, const std::size_t & j) const{
    assert(i < numRows_);
    assert(j < numCols_);
    return (*operand_)(rowStart_+i, colStart_+j);
  }
};
#endif


#ifdef PRESSIO_ENABLE_TPL_KOKKOS
template <typename MatrixType>
class SubspanExpr<
  MatrixType,
  std::enable_if_t<
    ::pressio::is_dense_matrix_kokkos<MatrixType>::value
    >
  >
{

  using traits = SubSpanTraits<SubspanExpr<MatrixType>>;
  using pair_t = typename traits::pair_type;
  using native_expr_t = typename traits::native_expr_type;
  using reference_t   = typename traits::reference_type;

private:
  MatrixType * operand_;
  std::size_t rowStart_ = {};
  std::size_t colStart_ = {};
  std::size_t endRow_ = {};
  std::size_t endCol_ = {};
  std::size_t numRows_ = {};
  std::size_t numCols_ = {};
  native_expr_t nativeExprObj_;

public:
  SubspanExpr(MatrixType & matObjIn,
	      const pair_t rowRangeIn,
	      const pair_t colRangeIn)
    : operand_(&matObjIn),
      rowStart_(std::get<0>(rowRangeIn)),
      colStart_(std::get<0>(colRangeIn)),
      endRow_(std::get<1>(rowRangeIn)-1),
      endCol_(std::get<1>(colRangeIn)-1),
      numRows_(endRow_ - rowStart_ + 1),
      numCols_(endCol_ - colStart_ + 1),
      nativeExprObj_(Kokkos::subview(*operand_,
				     std::make_pair(rowStart_, rowStart_+numRows_),
				     std::make_pair(colStart_, colStart_+numCols_)))
  {
    assert( rowStart_ >= 0 && rowStart_ < matObjIn.extent(0) );
    assert( std::get<1>(rowRangeIn) <= matObjIn.extent(0) );
    assert( colStart_ >= 0 && colStart_ < matObjIn.extent(1) );
    assert( std::get<1>(colRangeIn) <= matObjIn.extent(1) );

    // here the ranges are exclusive of the last index (like Kokkos and Python)
    // so the indices of the last row and col included are:
    assert(endRow_ >= rowStart_);
    assert(endCol_ >= colStart_);
  }

public:
  auto data() const { return operand_; }

  std::size_t extent(std::size_t i) const{
    if (i == 0) {
      return numRows_;
    } else if (i == 1) {
      return numCols_;
    } else {
      return 1;
    }
  }

  native_expr_t const & native() const{ return nativeExprObj_; }
  native_expr_t & native(){ return nativeExprObj_; }

  reference_t operator()(const std::size_t & i,
			 const std::size_t & j) const{
    assert(i < numRows_);
    assert(j < numCols_);
    return nativeExprObj_(i, j);
  }
};
#endif

}}}
#endif  // EXPRESSIONS_IMPL_SUBSPAN_CLASSES_HPP_

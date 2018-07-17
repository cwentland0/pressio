
#ifndef ROM_GALERKIN_IMPLICIT_JACOBIAN_POLICY_HPP_
#define ROM_GALERKIN_IMPLICIT_JACOBIAN_POLICY_HPP_

#include "rom_ConfigDefs.hpp"
#include "rom_incremental_solution_base.hpp"
#include "policies/base/ode_jacobian_policy_base.hpp"
#include "ode_jacobian_impl.hpp"

namespace rom{
namespace exp{

template<typename state_type,
	 typename jacobian_type,
	 typename model_type,
	 typename time_type,
	 typename sizer_type,
	 typename basis_t>
class romGalerkinImplicitJacobianPolicy 
  : public ode::policy::jacobianPolicyBase<
  romGalerkinImplicitJacobianPolicy<state_type, jacobian_type,
				    model_type, time_type,
				    sizer_type, basis_t> >,
    protected incrementalSolutionBase<romGalerkinImplicitJacobianPolicy,
				      state_type, jacobian_type,
				      model_type, time_type, sizer_type,
				      basis_t>
{

private:
  using base_t = ode::policy::jacobianPolicyBase<
  romGalerkinImplicitJacobianPolicy<state_type, jacobian_type,
				    model_type, time_type,
				    sizer_type, basis_t> >;

  using incr_base_t = incrementalSolutionBase<
    romGalerkinImplicitJacobianPolicy, state_type, jacobian_type,
    model_type, time_type, sizer_type, basis_t>;

private:
  using incr_base_t::y0ptr_;
  using incr_base_t::yFull_;

public:
  romGalerkinImplicitJacobianPolicy(const state_type & y0,
				    basis_t const & phi,
				    basis_t const & phiT)
    : incr_base_t(y0), phiPtr_(&phi), phiTPtr_(&phiT)
  {
    yFOM_.resize(phiPtr_->rows());
    J2_.resize(phiPtr_->rows(), phiPtr_->rows());
  }

  ~romGalerkinImplicitJacobianPolicy() = default;

private:
  basis_t const * phiPtr_;
  basis_t const * phiTPtr_;
  jacobian_type J2_;
  state_type yFOM_;
  
private:
  // enable if using types from core package
  template <typename U = state_type,
	    typename T = jacobian_type,
	    typename std::enable_if<
	      core::meta::is_coreVector<U>::value==true &&
	      core::meta::is_coreMatrixWrapper<T>::value==true
	    >::type * = nullptr>
  void computeImpl(const U & y,
		   T & J,
		   model_type & model,
		   time_type t,
		   time_type dt)
  {
    // reconstruct the solution
    //    yFull_ = y;//*y0ptr_ + y;

    if (J.rows()==0)
      J.resize(y.size(), y.size());

    *yFOM_.data() = *phiPtr_->data() * (*y.data());

    if (J2_.rows()==0)
      J2_.resize(yFOM_.size(), yFOM_.size());
    
    // first eval space jac
    model.jacobian( *yFOM_.data(), *J2_.data(), t);

    Eigen::MatrixXd JJ(*J2_.data());
    auto a = phiTPtr_->data();
    auto b = phiPtr_->data();
    auto res = (*a) * JJ * (*b);
    // std::cout << "resSize "
    // 	      << a->rows() << " " << a->cols() << " "
    // 	      << JJ.rows() << " " << JJ.cols() << " "
    // 	      << b->rows() << " " << b->cols() << " "
    // 	      << std::endl;

    (*J.data()) = res.sparseView();
    
    // update from time discrete residual
    ode::impl::implicit_euler_time_discrete_jacobian(J, dt);
  }  

private:
  friend base_t;
  friend incr_base_t;

};//end class

  
}//end namespace exp
}//end namespace rom
#endif 

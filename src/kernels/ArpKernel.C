#include "ArpKernel.h"

template<>
InputParameters validParams<ArpKernel>()
{
  InputParameters params = validParams<Kernel>();

  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("em", "The electron density");
  // params.addRequiredCoupledVar("mean_en", "The electront temperature");
  return params;
}

ArpKernel::ArpKernel(const InputParameters & parameters) :
  Kernel(parameters),

  _em(coupledValue("em")),
  _grad_em(coupledGradient("em")),
  _em_id(coupled("em")),
  _grad_potential(coupledGradient("potential")),
  _potential_id(coupled("potential")),

  // Material Properties

  _muem(getMaterialProperty<Real>("muem")),
  _diffem(getMaterialProperty<Real>("diffem")),
  _muip(getMaterialProperty<Real>("muip")),
  _diffip(getMaterialProperty<Real>("diffip")),
  _Ar(getMaterialProperty<Real>("Ar")),
  _Eiz(getMaterialProperty<Real>("Eiz")),
  _rate_coeff_ion(getMaterialProperty<Real>("rate_coeff_ion")),

  // Kernel members

  _alpha(0.0),
  _Pe(0.0),
  _vd_mag(0.0),
  _delta(0.0),
  _em_flux(0.0,0.0,0.0),
  _d_em_flux_d_em(0.0,0.0,0.0),
  _d_em_flux_d_potential(0.0,0.0,0.0)
{}

ArpKernel::~ArpKernel()
{}

Real
ArpKernel::computeQpResidual()
{
  // _vd_mag = std::abs(_muip[_qp]*_grad_potential[_qp].norm());
  // _Pe = _vd_mag*_current_elem->hmax()/_diffip[_qp];
  // _alpha = std::min(1.0,_Pe/6.0);
  // _delta = _alpha*_vd_mag*_current_elem->hmax()/2.0;
 
  return -_grad_test[_i][_qp]*std::exp(_u[_qp])*(_muip[_qp]*-_grad_potential[_qp]-_diffip[_qp]*_grad_u[_qp])
    -_test[_i][_qp]*_rate_coeff_ion[_qp]*std::exp(-_Eiz[_qp]/(_grad_potential[_qp].norm()+std::numeric_limits<double>::epsilon()))*(-_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp]).norm(); // Reaction. Townsend coefficient formulation
    // -_grad_test[_i][_qp]*(-_delta*std::exp(_u[_qp])*_grad_u[_qp]); // Diffusion stabilization
}

Real
ArpKernel::computeQpJacobian()
{
  // _vd_mag = std::abs(_muip[_qp]*_grad_potential[_qp].norm());
  // _Pe = _vd_mag*_current_elem->hmax()/_diffip[_qp];
  // _alpha = std::min(1.0,_Pe/6.0);
  // _delta = _alpha*_vd_mag*_current_elem->hmax()/2.0;

  return -_grad_test[_i][_qp]*(_muip[_qp]*-_grad_potential[_qp]*std::exp(_u[_qp])*_phi[_j][_qp]-_diffip[_qp]*(std::exp(_u[_qp])*_grad_phi[_j][_qp]+std::exp(_u[_qp])*_phi[_j][_qp]*_grad_u[_qp]))
    -_grad_test[_i][_qp]*(-_delta*(std::exp(_u[_qp])*_grad_phi[_j][_qp]+std::exp(_u[_qp])*_phi[_j][_qp]*_grad_u[_qp])); // Diffusion stabilization
}

Real
ArpKernel::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _potential_id) {
    _em_flux = -_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp];
    _d_em_flux_d_potential = -_muem[_qp]*-_grad_phi[_j][_qp]*std::exp(_em[_qp]);

    return -_grad_test[_i][_qp]*(_muip[_qp]*-_grad_phi[_j][_qp]*std::exp(_u[_qp]))
      -_test[_i][_qp]*_rate_coeff_ion[_qp]*(std::exp(-_Eiz[_qp]/(std::numeric_limits<double>::epsilon()+_grad_potential[_qp].norm()))*_Eiz[_qp]*_grad_potential[_qp]*_grad_phi[_j][_qp]/(std::numeric_limits<double>::epsilon()+std::pow(_grad_potential[_qp].norm(),3))*(-_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp]).norm() + std::exp(-_Eiz[_qp]/(std::numeric_limits<double>::epsilon()+_grad_potential[_qp].norm()))*_em_flux*_d_em_flux_d_potential/(_em_flux.norm()+std::numeric_limits<double>::epsilon())); // Reaction. Townsend coefficient formulation
  }

  else if (jvar == _em_id) {
    _em_flux = -_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp];
    _d_em_flux_d_em = -_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])*_phi[_j][_qp]-_diffem[_qp]*(std::exp(_em[_qp])*_grad_phi[_j][_qp]+std::exp(_em[_qp])*_phi[_j][_qp]*_grad_em[_qp]);

    return -_test[_i][_qp]*_rate_coeff_ion[_qp]*std::exp(-_Eiz[_qp]/(std::numeric_limits<double>::epsilon()+_grad_potential[_qp].norm()))*_em_flux*_d_em_flux_d_em/(_em_flux.norm()+std::numeric_limits<double>::epsilon()); // Reaction. Townsend coefficient formulation

  }

  else {
    return 0.0;
  }
}


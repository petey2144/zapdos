#include "IonKernelIntTD.h"

template<>
InputParameters validParams<IonKernelIntTD>()
{
  InputParameters params = validParams<Kernel>();

  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("em", "The electron density");
  // params.addRequiredCoupledVar("mean_en", "The electront temperature");
  return params;
}

IonKernelIntTD::IonKernelIntTD(const InputParameters & parameters) :
  Kernel(parameters),

  _em(coupledValue("em")),
  _grad_em(coupledGradient("em")),
  _em_id(coupled("em")),
  _grad_potential(coupledGradient("potential")),
  _potential_id(coupled("potential")),
  // _mean_en(coupledValue("mean_en")),
  // _mean_en_id(coupled("mean_en")),

  // Material Properties

  _muem(getMaterialProperty<Real>("muem")),
  _diffem(getMaterialProperty<Real>("diffem")),
  _muip(getMaterialProperty<Real>("muip")),
  _diffip(getMaterialProperty<Real>("diffip")),
  _Ar(getMaterialProperty<Real>("Ar")),
  _rate_coeff_ion(getMaterialProperty<Real>("rate_coeff_ion")),

  // Kernel members

  _alpha(0.0),
  _Pe(0.0),
  _vd_mag(0.0),
  _delta(0.0)
{}

IonKernelIntTD::~IonKernelIntTD()
{}

Real
IonKernelIntTD::computeQpResidual()
{
  _vd_mag = std::abs(_muip[_qp]*_grad_potential[_qp].norm());
  _Pe = _vd_mag*_current_elem->hmax()/_diffip[_qp];
  _alpha = std::min(1.0,_Pe/6.0);
  _delta = _alpha*_vd_mag*_current_elem->hmax()/2.0;
 
  return -_grad_test[_i][_qp]*std::exp(_u[_qp])*(_muip[_qp]*-_grad_potential[_qp]-_diffip[_qp]*_grad_u[_qp])
    -_test[_i][_qp]*_rate_coeff_ion[_qp]*(-_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp]).norm(); // Reaction. Townsend coefficient formulation
    // -_grad_test[_i][_qp]*(-_delta*std::exp(_u[_qp])*_grad_u[_qp]); // Diffusion stabilization
}

// Real
// IonKernelIntTD::computeQpJacobian()
// {
//   _vd_mag = std::abs(_muip[_qp]*_grad_potential[_qp].norm());
//   _Pe = _vd_mag*_current_elem->hmax()/_diffip[_qp];
//   _alpha = std::min(1.0,_Pe/6.0);
//   _delta = _alpha*_vd_mag*_current_elem->hmax()/2.0;

//   return -_grad_test[_i][_qp]*(_muip[_qp]*-_grad_potential[_qp]*std::exp(_u[_qp])*_phi[_j][_qp]-_diffip[_qp]*(std::exp(_u[_qp])*_grad_phi[_j][_qp]+std::exp(_u[_qp])*_phi[_j][_qp]*_grad_u[_qp]))
//     -_grad_test[_i][_qp]*(-_delta*(std::exp(_u[_qp])*_grad_phi[_j][_qp]+std::exp(_u[_qp])*_phi[_j][_qp]*_grad_u[_qp])); // Diffusion stabilization
// }

// Real
// IonKernelIntTD::computeQpOffDiagJacobian(unsigned int jvar)
// {
//   if (jvar == _potential_id) {
//     return -_grad_test[_i][_qp]*(_muip[_qp]*-_grad_phi[_j][_qp]*std::exp(_u[_qp]));
//   }

//   else if (jvar == _em_id) {
//     return -_test[_i][_qp]*_rate_coeff_ion[_qp]*_Ar[_qp]*((-3.0/2*_Eiz[_qp]*std::exp(2.0*_em[_qp]-_mean_en[_qp])+std::exp(_em[_qp]))*std::exp(-3.0/2*_Eiz[_qp]*std::exp(_em[_qp]-_mean_en[_qp]))*_phi[_j][_qp]);
//   }

//   else if (jvar == _mean_en_id) {
//    return -_test[_i][_qp]*_rate_coeff_ion[_qp]*_Ar[_qp]*std::exp(_em[_qp])*std::exp(-_Eiz[_qp]/(2.0/3*std::exp(_mean_en[_qp]-_em[_qp])))*3.0/2*_Eiz[_qp]*std::exp(_em[_qp]-_mean_en[_qp])*_phi[_j][_qp]; // Reaction coefficient formulation
//   }

//   else {
//     return 0.0;
//   }
// }


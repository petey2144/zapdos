#include "DCElectronBC.h"

template<>
InputParameters validParams<DCElectronBC>()
{
    InputParameters params = validParams<IntegratedBC>();
    params.addRequiredCoupledVar("ip","The ion density");
    params.addRequiredCoupledVar("potential","The electric potential");
    params.addCoupledVar("mean_en", "The mean energy.");
    return params;
}

DCElectronBC::DCElectronBC(const InputParameters & parameters) :
  IntegratedBC(parameters),

// Coupled Variables
  _ip_var(*getVar("ip",0)),
  _ip(coupledValue("ip")),
  _ip_id(coupled("ip")),
  _grad_ip(coupledGradient("ip")),
  _grad_potential(coupledGradient("potential")),
  _potential_id(coupled("potential")),
  _mean_en(isCoupled("mean_en") ? coupledValue("mean_en") : _zero),
  _mean_en_id(isCoupled("mean_en") ? coupled("mean_en") : 12345),

  _muem(getMaterialProperty<Real>("muem")),
  _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
  _mem(getMaterialProperty<Real>("mem")),
  _se_coeff(getMaterialProperty<Real>("se_coeff")),
  _muip(getMaterialProperty<Real>("mu"+_ip_var.name())),
  _vthermal_em(getMaterialProperty<Real>("vthermal_em")),
  _a(0.5),
  _b(0.5)
{}

Real
DCElectronBC::computeQpResidual()
{
  if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
    _a = 1.0;
    _b = 0.0;
  }
  else {
    _a = 0.0;
    _b = 1.0;
  }
  return _test[_i][_qp]*(_a*-_muem[_qp]*-_grad_potential[_qp]*std::exp(_u[_qp])*_normals[_qp]
			 -_b*_se_coeff[_qp]*_muip[_qp]*-_grad_potential[_qp]*std::exp(_ip[_qp])*_normals[_qp]);
}

Real
DCElectronBC::computeQpJacobian()
{
  if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
    _a = 1.0;
    _b = 0.0;
  }
  else {
    _a = 0.0;
    _b = 1.0;
  }
  Real _actual_mean_en = std::exp(_mean_en[_qp]-_u[_qp]);
  Real _d_actual_mean_en_d_em = _actual_mean_en*-_phi[_j][_qp];
  Real _d_muem_d_em = _d_muem_d_actual_mean_en[_qp]*_d_actual_mean_en_d_em;

  return _test[_i][_qp]*(_a*-_muem[_qp]*-_grad_potential[_qp]*std::exp(_u[_qp])*_phi[_j][_qp]*_normals[_qp] + _a*-_d_muem_d_em*-_grad_potential[_qp]*std::exp(_u[_qp])*_normals[_qp]);
}

Real
DCElectronBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _potential_id) {
    if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
      _a = 1.0;
      _b = 0.0;
    }
    else {
      _a = 0.0;
      _b = 1.0;
    }
    return _test[_i][_qp]*(_a*-_muem[_qp]*-_grad_phi[_j][_qp]*std::exp(_u[_qp])*_normals[_qp]
			  -_b*_se_coeff[_qp]*_muip[_qp]*-_grad_phi[_j][_qp]*std::exp(_ip[_qp])*_normals[_qp]);
  }

  else if (jvar == _ip_id) {
    if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
      _a = 1.0;
      _b = 0.0;
    }
    else {
      _a = 0.0;
      _b = 1.0;
    }
    return _test[_i][_qp]*(-_b*_se_coeff[_qp]*_muip[_qp]*-_grad_potential[_qp]*std::exp(_ip[_qp])*_phi[_j][_qp]*_normals[_qp]);
  }

  else if (jvar == _mean_en_id) {
    if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
      _a = 1.0;
      _b = 0.0;
    }
    else {
      _a = 0.0;
      _b = 1.0;
    }
    Real _actual_mean_en = std::exp(_mean_en[_qp]-_u[_qp]);
    Real _d_actual_mean_en_d_mean_en = _actual_mean_en*_phi[_j][_qp];
    Real _d_muem_d_mean_en = _d_muem_d_actual_mean_en[_qp]*_d_actual_mean_en_d_mean_en;

    return _test[_i][_qp]*(_a*-_d_muem_d_actual_mean_en[_qp]*-_grad_potential[_qp]*std::exp(_u[_qp])*_normals[_qp]);
  }

  else {
    return 0.0;
  }
}

/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "NeumannCircuitVoltage.h"
#include "Function.h"

template<>
InputParameters validParams<NeumannCircuitVoltage>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<FunctionName>("function", "The function.");
  params.addRequiredParam<UserObjectName>("data_provider","The name of the UserObject that can provide some data to materials, bcs, etc.");
  params.addRequiredCoupledVar("ip","The ion density.");

  return params;
}

NeumannCircuitVoltage::NeumannCircuitVoltage(const InputParameters & parameters) :
    IntegratedBC(parameters),
    _V_bat(getFunction("function")),
    _data(getUserObject<ProvideMobility>("data_provider")),
    _ip_var(*getVar("ip",0)),
    _ip(coupledValue("ip")),
    _ip_id(coupled("ip")),
    _se_coeff(getMaterialProperty<Real>("se_coeff")),
    _muip(getMaterialProperty<Real>("mu"+_ip_var.name())),
    _eps(getMaterialProperty<Real>("eps"))
{
}

Real
NeumannCircuitVoltage::computeQpResidual()
{
  return _test[_i][_qp] * _eps[_qp] * (_u[_qp] + _V_bat.value(_t, _q_point[_qp]))/((1+_se_coeff[_qp])*_data.coulomb_charge()*_muip[_qp]*std::exp(_ip[_qp])*_data.ballast_resist()*_data.electrode_area());
}

Real
NeumannCircuitVoltage::computeQpJacobian()
{
  return _test[_i][_qp] * _eps[_qp] * (_phi[_j][_qp])/((1+_se_coeff[_qp])*_data.coulomb_charge()*_muip[_qp]*std::exp(_ip[_qp])*_data.ballast_resist()*_data.electrode_area());
}

Real
NeumannCircuitVoltage::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _ip_id) {

    return _test[_i][_qp] * _eps[_qp] * (_u[_qp] + _V_bat.value(_t, _q_point[_qp]))/((1+_se_coeff[_qp])*_data.coulomb_charge()*_muip[_qp]*_data.ballast_resist()*_data.electrode_area())*std::exp(-_ip[_qp])*-_phi[_j][_qp];
  }
  else
    return 0.0;
}

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

#include "CoeffParamDiffusion.h"


template<>
InputParameters validParams<CoeffParamDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addRequiredParam<Real>("D","The diffusivity coefficient.");
  return params;
}


CoeffParamDiffusion::CoeffParamDiffusion(const InputParameters & parameters) :
    Diffusion(parameters),

    // Input Parameters

    _D(getParam<Real>("D"))

{
}

CoeffParamDiffusion::~CoeffParamDiffusion()
{
}

Real
CoeffParamDiffusion::computeQpResidual()
{
  return _D * _grad_u[_qp] * _grad_test[_i][_qp];
}

Real
CoeffParamDiffusion::computeQpJacobian()
{
  return _D * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
}

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

#ifndef MULTIPLIEDVALUEBC_H
#define MULTIPLIEDVALUEBC_H

#include "NodalBC.h"

//Forward Declarations
class MultipliedValueBC;

template<>
InputParameters validParams<MultipliedValueBC>();

/**
 * Implements a simple coupled boundary condition where u=v on the boundary.
 */
class MultipliedValueBC : public NodalBC
{
public:
  MultipliedValueBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const VariableValue & _v;

  /// The id of the coupled variable
  unsigned int _v_num;

  Real _mult;
};

#endif //MULTIPLIEDVALUEBC_H

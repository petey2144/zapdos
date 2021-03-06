#ifndef RFELECTRONBC_H
#define RFELECTRONBC_H

#include "IntegratedBC.h"

class RFElectronBC;

template<>
InputParameters validParams<RFElectronBC>();

class RFElectronBC : public IntegratedBC
{
public:

  RFElectronBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Input Parameters

  /* const Real & _se_coeff; */

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _mem;
  const MaterialProperty<Real> & _se_coeff;
  const MaterialProperty<Real> & _muip;
  const MaterialProperty<Real> & _vthermal_em;

  Real _a;
  Real _b;

  // Coupled variables
  
  const VariableValue & _Arp;
  unsigned int _Arp_id;
  const VariableGradient & _grad_Arp;
  const VariableGradient & _grad_potential;
  unsigned int _potential_id;
};

#endif //RFELECTRONBC_H

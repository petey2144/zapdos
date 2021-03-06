#ifndef ELECTRONTOTALFLUX_H
#define ELECTRONTOTALFLUX_H

#include "AuxKernel.h"

class ElectronTotalFlux;

template<>
InputParameters validParams<ElectronTotalFlux>();

class ElectronTotalFlux : public AuxKernel
{
public:
  ElectronTotalFlux(const InputParameters & parameters);
  
  virtual ~ElectronTotalFlux() {}
  
protected:

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _diffem;
  const VariableValue & _em;
  const VariableGradient & _grad_potential;
  const VariableGradient & _grad_em;
  virtual Real computeValue();

};

#endif //ELECTRONTOTALFLUX_H

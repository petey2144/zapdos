#ifndef ELECTRONDIFFUSIVEFLUX_H
#define ELECTRONDIFFUSIVEFLUX_H

#include "AuxKernel.h"

class ElectronDiffusiveFlux;

template<>
InputParameters validParams<ElectronDiffusiveFlux>();

class ElectronDiffusiveFlux : public AuxKernel
{
public:
  ElectronDiffusiveFlux(const InputParameters & parameters);
  
  virtual ~ElectronDiffusiveFlux() {}
  
protected:

  virtual Real computeValue();
  const MaterialProperty<Real> & _diffem;
  const VariableValue & _em;
  const VariableGradient & _grad_em;
};

#endif //ELECTRONDIFFUSIVEFLUX_H

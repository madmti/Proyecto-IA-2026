#pragma once

#include "algo_exh.hpp"
#include "instancias.hpp"

#include <vector>

namespace mri {

// Resultado de resolver una instancia.
struct Resultado {
  std::vector<int> inicial;
  double fInicial = 0.0;
  std::vector<int> final;
  double fFinal = 0.0;
  double segundos = 0.0;
};

// Escribe el resultado de una instancia (umbrales, f y tiempo).
void escribirResultado(const Instancia &instancia, const Resultado &resultado);

// Escribe el optimo global de una instancia (umbrales, f y tiempo).
void escribirOptimo(const Instancia &instancia, const Optimo &optimo);

} // namespace mri

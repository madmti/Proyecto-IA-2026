#pragma once

#include "instancias.hpp"
#include "otsu.hpp"

#include <vector>

namespace algo {

// Optimo global de una instancia.
struct Optimo {
	std::vector<int> umbrales; // Una solucion optima (puede haber varias).
	double f = 0.0;			   // Valor optimo de f(t).
	double segundos = 0.0;	   // Tiempo de la busqueda.
};

// Busqueda exhaustiva: evalua todas las combinaciones factibles
// 1 <= t1 < ... < tk <= 254 y devuelve la de mayor f(t).
std::vector<int> buscarOptimoExhaustivo(const io::Instancia &instancia,
							  const otsu::TablaOtsu &tabla);

} // namespace algo

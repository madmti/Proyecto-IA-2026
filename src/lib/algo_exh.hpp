#pragma once

#include "instancias.hpp"
#include "otsu.hpp"

#include <vector>

namespace algo {

// Optimo global de una instancia.
struct Optimo {
	std::vector<std::vector<int>> soluciones; // Todas las soluciones optimas.
	double f = 0.0;							  // Valor optimo de f(t).
	double segundos = 0.0;					  // Tiempo de la busqueda.
};

// Busqueda exhaustiva: evalua todas las combinaciones factibles
// 1 <= t1 < ... < tk <= 254 y devuelve todas las de mayor f(t) (empates con
// tolerancia 1e-12), en orden lexicografico.
std::vector<std::vector<int>>
buscarOptimoExhaustivo(const io::Instancia &instancia,
					   const otsu::TablaOtsu &tabla);

} // namespace algo

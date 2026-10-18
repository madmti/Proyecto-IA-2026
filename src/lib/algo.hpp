#pragma once

#include "io.hpp"
#include "otsu.hpp"

#include <vector>

namespace algo {

// Genera una solucion inicial factible:
// k umbrales enteros en [1, 254], estrictamente crecientes.
// Recibe la tabla para poder usar f(t) si el metodo es greedy.
// La misma semilla produce la misma solucion.
std::vector<int> generarSolucionInicial(const io::Instancia &instancia,
										const otsu::TablaOtsu &tabla,
										unsigned int semilla);

// Mejora una solucion factible mediante movimientos.
// Todos los movimientos deben mantener la factibilidad:
// 1 <= t1 < t2 < ... < tk <= 254.
std::vector<int> repararSolucion(const std::vector<int> &inicial,
								 const otsu::TablaOtsu &tabla,
								 unsigned int semilla);

// Busqueda exhaustiva: evalua todas las combinaciones factibles
// 1 <= t1 < ... < tk <= 254 y devuelve todas las de mayor f(t) (empates con
// tolerancia 1e-12), en orden lexicografico.
std::vector<std::vector<int>>
buscarOptimoExhaustivo(const io::Instancia &instancia,
					   const otsu::TablaOtsu &tabla);

} // namespace algo

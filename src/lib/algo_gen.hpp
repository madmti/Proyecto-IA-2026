#pragma once

#include "instancias.hpp"
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

} // namespace algo

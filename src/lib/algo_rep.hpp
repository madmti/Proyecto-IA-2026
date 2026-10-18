#pragma once

#include "otsu.hpp"

#include <vector>

namespace mri {

// Mejora una solucion factible mediante movimientos.
// Todos los movimientos deben mantener la factibilidad:
// 1 <= t1 < t2 < ... < tk <= 254.
std::vector<int> repararSolucion(const std::vector<int> &inicial,
								 const TablaOtsu &tabla, unsigned int semilla);

} // namespace mri

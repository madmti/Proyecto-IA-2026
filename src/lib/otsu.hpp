#pragma once

#include "imagenes.hpp"

#include <array>
#include <vector>

namespace otsu {

// Valores precalculados del histograma para evaluar f(t) en O(k).
struct TablaOtsu {
	std::array<double, 256> acumP{};  // acumP[g]: suma de p(0..g).
	std::array<double, 256> acumGP{}; // acumGP[g]: suma de g * p(0..g).
	double muT = 0.0;				  // Intensidad media de toda la imagen.
};

// Precalcula las sumas acumuladas una sola vez por instancia.
TablaOtsu prepararOtsu(const img::Histograma &histograma, unsigned long long N);

// Varianza entre clases de Otsu para los umbrales dados.
// Aporte w_j * (mu_j - muT)^2 de la clase con intensidades [a, b).
// Maneja los bordes: a <= 0 es el inicio del histograma, b >= 256 es el final
// y una clase vacia (w_j = 0) o con b <= a aporta 0.
double fTerm(const TablaOtsu &tabla, int a, int b);

double funObj(const std::vector<int> &umbrales, const TablaOtsu &tabla);

} // namespace otsu

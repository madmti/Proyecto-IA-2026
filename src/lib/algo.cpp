#include "algo.hpp"

#include <random>
#include <vector>

namespace algo {

/*=================================================================
					Algoritmo Generador
=================================================================*/

void greedy(std::vector<int> &umbrales, double &facc, int k,
			const otsu::TablaOtsu &tabla, std::mt19937 &gen) {
	int size = static_cast<int>(umbrales.size());
	if (size >= k + 1) {
		facc += otsu::fTerm(tabla, umbrales.back(), 255);
		return;
	}
	int cotainf = umbrales.back() + 1, cota_sup = 254 - k + size;
	double max = -1;
	std::vector<int> candidatos;
	for (int um = cotainf; um <= cota_sup; um++) {
		double aporte = otsu::fTerm(tabla, umbrales.back(), um);
		if (aporte > max) {
			max = aporte;
			candidatos = {um};
		} else if (aporte == max) {
			candidatos.push_back(um);
		}
	};

	// Desempate
	std::uniform_int_distribution<size_t> dist(0, candidatos.size() - 1);
	umbrales.push_back(candidatos[dist(gen)]);
	greedy(umbrales, facc, k, tabla, gen);
}

std::vector<int> generarSolucionInicial(const io::Instancia &instancia,
										const otsu::TablaOtsu &tabla,
										unsigned int semilla) {
	std::vector<int> umbrales = {0};
	double f = 0.0;
	std::mt19937 gen(semilla);
	greedy(umbrales, f, instancia.k, tabla, gen);
	umbrales.erase(umbrales.begin());
	return umbrales;
}

/*=================================================================
					Algoritmo Reparador
=================================================================*/

std::vector<int> repararSolucion(const std::vector<int> &inicial,
								 const otsu::TablaOtsu &, unsigned int) {
	return inicial;
}

/*=================================================================
					Algoritmo Completo
=================================================================*/

constexpr double EPS_EMPATE = 1e-12;

void backtracking(std::vector<int> umbrales, int next, double facc, int k,
				  const otsu::TablaOtsu &tabla,
				  std::vector<std::vector<int>> &sols, double &maxf) {
	umbrales.push_back(next);
	int size = static_cast<int>(umbrales.size());
	if (size >= k + 1) {
		facc += otsu::fTerm(tabla, umbrales.back(), 256);
		double diff = facc - maxf;
		if (diff > EPS_EMPATE) {
			umbrales.erase(umbrales.begin());
			sols = {umbrales};
			maxf = facc;
		} else if (std::abs(diff) <= EPS_EMPATE) {
			umbrales.erase(umbrales.begin());
			sols.push_back(umbrales);
		}
		return;
	}

	int cotainf = umbrales.back() + 1, cota_sup = 254 - k + size;
	for (int um = cotainf; um <= cota_sup; um++) {
		double aporte = otsu::fTerm(tabla, umbrales.back(), um);
		backtracking(umbrales, um, facc + aporte, k, tabla, sols, maxf);
	}
};

std::vector<std::vector<int>>
buscarOptimoExhaustivo(const io::Instancia &instancia,
					   const otsu::TablaOtsu &tabla) {

	std::vector<std::vector<int>> soluciones;
	double maxf = -1.0;
	backtracking({}, 0, 0.0, instancia.k, tabla, soluciones, maxf);
	return soluciones;
}

} // namespace algo

#include "algo_gen.hpp"
#include "otsu.hpp"
#include <random>
#include <vector>

namespace algo {

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

} // namespace algo

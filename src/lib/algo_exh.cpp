#include "algo_exh.hpp"
#include "otsu.hpp"
#include <vector>

namespace algo {

void backtracking(std::vector<int> umbrales, int next, double facc, int k,
				  const otsu::TablaOtsu &tabla,
				  std::vector<std::vector<int>> &sols, double &maxf) {
	umbrales.push_back(next);
	int size = static_cast<int>(umbrales.size());
	if (size >= k + 1) {
		facc += otsu::fTerm(tabla, umbrales.back(), 255);
		if (facc > maxf) {
			umbrales.erase(umbrales.begin());
			sols = {umbrales};
		} else if (facc == maxf) {
			umbrales.erase(umbrales.begin());
			sols.push_back(umbrales);
			maxf = facc;
		}
		return;
	}

	int cotainf = umbrales.back() + 1, cota_sup = 254 - k + size;
	for (int um = cotainf; um <= cota_sup; um++) {
		double aporte = otsu::fTerm(tabla, umbrales.back(), um);
		backtracking(umbrales, um, facc + aporte, k, tabla, sols, maxf);
	}
};

std::vector<int> buscarOptimoExhaustivo(const io::Instancia &instancia,
										const otsu::TablaOtsu &tabla) {

	std::vector<std::vector<int>> soluciones;
	double maxf = 0.0;
	backtracking({}, 0, 0.0, instancia.k, tabla, soluciones, maxf);
	return soluciones.back();
}

} // namespace algo

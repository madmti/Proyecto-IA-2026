#include "algo_gen.hpp"
#include <cassert>
#include <vector>

namespace algo {

std::vector<int> generarSolucionInicial(const io::Instancia &instancia,
										const otsu::TablaOtsu &, unsigned int) {
	std::vector<int> sol(instancia.k, 0);
	for (int i = 0; i < instancia.k; i++) {
		sol[i] = i + 1;
	}
	return sol;
}

} // namespace algo

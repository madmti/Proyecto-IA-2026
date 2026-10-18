#include "algo_gen.hpp"
#include <cassert>
#include <vector>

namespace mri {

std::vector<int> generarSolucionInicial(const Instancia &instancia,
										const TablaOtsu &, unsigned int) {
	std::vector<int> sol(instancia.k, 0);
	for (int i = 0; i < instancia.k; i++) {
		sol[i] = i + 1;
	}
	return sol;
}

} // namespace mri

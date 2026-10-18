#include "algo_exh.hpp"
#include <vector>

namespace algo {

std::vector<int> buscarOptimoExhaustivo(const io::Instancia &instancia,
										const otsu::TablaOtsu &tabla) {
	return std::vector<int>(instancia.k, 0);
}

} // namespace algo

#include "algo_exh.hpp"

namespace algo {

Optimo buscarOptimoExhaustivo(const io::Instancia &instancia,
							  const otsu::TablaOtsu &) {
	Optimo optimo;
	optimo.umbrales = std::vector<int>(instancia.k, 0);
	return optimo;
}

} // namespace algo

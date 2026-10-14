#include "algo_exh.hpp"

namespace mri {

Optimo buscarOptimoExhaustivo(const Instancia &instancia, const TablaOtsu &) {
  Optimo optimo;
  optimo.umbrales = std::vector<int>(instancia.k, 0);
  return optimo;
}

} // namespace mri

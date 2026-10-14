#include "algo_gen.hpp"

namespace mri {

std::vector<int> generarSolucionInicial(const Instancia &instancia,
                                        const TablaOtsu &, unsigned int) {
  return std::vector<int>(instancia.k, 0);
}

} // namespace mri

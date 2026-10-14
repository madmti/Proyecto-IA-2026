#include "lib/algo_exh.hpp"
#include "lib/instancias.hpp"
#include "lib/otsu.hpp"
#include "lib/salida.hpp"

#include <exception>
#include <iostream>
#include <vector>

using namespace std;

const char *const CARPETA_INSTANCIAS = "data/preprocesado/instancias";

// Calcula el optimo global de una instancia.
mri::Optimo resolver(const mri::Instancia &instancia) {
  const auto tabla = mri::prepararOtsu(instancia.histograma, instancia.N);
  return mri::buscarOptimoExhaustivo(instancia, tabla);
}

int main() {
  vector<mri::Instancia> instancias;
  try {
    instancias = mri::cargarInstancias(CARPETA_INSTANCIAS);
  } catch (const exception &e) {
    cerr << "[e] error leyendo instancia: " << e.what() << "\n";
    return 1;
  }
  cout << "[i] Cargadas " << instancias.size() << " instancias desde "
       << CARPETA_INSTANCIAS << "\n";

  for (const auto &instancia : instancias) {
    const auto optimo = resolver(instancia);
    mri::escribirOptimo(instancia, optimo);
  }
  return 0;
}

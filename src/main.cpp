#include "lib/instancias.hpp"

#include <exception>
#include <iostream>
#include <vector>

using namespace std;

const char *const CARPETA_INSTANCIAS = "data/preprocesado/instancias";

int main() {
  try {
    vector<mri::Instancia> instancias;
    for (const auto &ruta : mri::listarInstancias(CARPETA_INSTANCIAS))
      instancias.push_back(mri::leerInstancia(ruta));

    cout << "[i] Cargadas " << instancias.size() << " instancias desde "
         << CARPETA_INSTANCIAS << "\n";
  } catch (const exception &e) {
    cerr << "[e] error leyendo instancia: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

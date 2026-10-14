#include "lib/algo_gen.hpp"
#include "lib/algo_rep.hpp"
#include "lib/instancias.hpp"
#include "lib/otsu.hpp"
#include "lib/salida.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <vector>

using namespace std;

const char *const CARPETA_INSTANCIAS = "data/preprocesado/instancias";
const unsigned int SEMILLA = 42;

// Resuelve una instancia especifica
mri::Resultado resolver(const mri::Instancia &instancia, unsigned int semilla) {
  mri::Resultado resultado;

  const auto tabla = mri::prepararOtsu(instancia.histograma, instancia.N);

  const auto inicio = chrono::steady_clock::now();

  resultado.inicial = mri::generarSolucionInicial(instancia, tabla, semilla);
  resultado.fInicial = mri::funObj(resultado.inicial, tabla);

  resultado.final = mri::repararSolucion(resultado.inicial, tabla, semilla);
  resultado.fFinal = mri::funObj(resultado.final, tabla);

  const chrono::duration<double> transcurrido =
      chrono::steady_clock::now() - inicio;
  resultado.segundos = transcurrido.count();

  return resultado;
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
    const auto resultado = resolver(instancia, SEMILLA);
    mri::escribirResultado(instancia, resultado);
  }
  return 0;
}

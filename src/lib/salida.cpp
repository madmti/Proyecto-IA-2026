#include "salida.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace mri {
namespace {
string formatearUmbrales(const vector<int> &umbrales) {
  ostringstream texto;
  texto << "{";
  for (size_t i = 0; i < umbrales.size(); ++i)
    texto << (i ? ", " : "") << umbrales[i];
  texto << "}";
  return texto.str();
}
} // namespace

void escribirResultado(const Instancia &instancia, const Resultado &resultado) {
  const string inicial = formatearUmbrales(resultado.inicial);
  const string final = formatearUmbrales(resultado.final);
  const int ancho = static_cast<int>(max(inicial.size(), final.size()));

  cout << "[i] " << instancia.ruta.filename().string() << " (k=" << instancia.k
       << ")\n"
       << fixed << setprecision(4) << "    inicial  t=" << left << setw(ancho)
       << inicial << "  f=" << resultado.fInicial << "\n"
       << "    final    t=" << left << setw(ancho) << final
       << "  f=" << resultado.fFinal << "  (" << showpos
       << resultado.fFinal - resultado.fInicial << noshowpos << ")  "
       << setprecision(3) << resultado.segundos << " s\n";
}

// TODO: escribir umbrales, f y tiempo del optimo.
void escribirOptimo(const Instancia &, const Optimo &) {}

} // namespace mri

#pragma once

#include "algo_exh.hpp"
#include "instancias.hpp"

#include <filesystem>
#include <vector>

namespace mri {

// Resultado de resolver una instancia.
struct Resultado {
  unsigned int semilla = 0;
  std::vector<int> inicial;
  double fInicial = 0.0;
  std::vector<int> final;
  double fFinal = 0.0;
  double segundos = 0.0;
};

// Imprime por consola el resultado de una instancia (umbrales, f y tiempo).
void imprimirResultado(const Instancia &instancia, const Resultado &resultado);

// Imprime por consola el optimo global de una instancia (umbrales, f y tiempo).
void imprimirOptimo(const Instancia &instancia, const Optimo &optimo);

// Guardan un archivo de texto por instancia en <carpeta>/<instancia>.txt
// (ej: data/resultados/soluciones/imagen_01_k2.txt). Crean la carpeta si no existe,
// reemplazan el archivo si ya existe y devuelven la ruta escrita.
// Lanzan std::runtime_error si no se puede escribir.
std::filesystem::path guardarResultado(const Instancia &instancia,
                                       const Resultado &resultado,
                                       const std::filesystem::path &carpeta);
std::filesystem::path guardarOptimo(const Instancia &instancia,
                                    const Optimo &optimo,
                                    const std::filesystem::path &carpeta);

} // namespace mri

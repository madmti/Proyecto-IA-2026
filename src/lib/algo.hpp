#pragma once

#include "io.hpp"
#include "otsu.hpp"

#include <vector>

namespace algo {

// Parametros configurables del algoritmo. Los valores por defecto son los
// usados en la configuracion base (main sin argumentos).
struct Parametros {
	int tenencia = 10;		   // T: la lista tabu guarda k + T pares.
	int iteraciones = 1000;	   // Iteraciones de Tabu Search.
	unsigned int semilla = 42; // Semilla de los generadores aleatorios.
};

// Genera una solucion inicial factible:
// k umbrales enteros en [1, 254], estrictamente crecientes.
// Recibe la tabla para poder usar f(t) si el metodo es greedy.
// La misma semilla produce la misma solucion.
std::vector<int> generarSolucionInicial(const io::Instancia &instancia,
										const otsu::TablaOtsu &tabla,
										unsigned int semilla);

// Mejora una solucion factible mediante movimientos.
// Todos los movimientos deben mantener la factibilidad:
// 1 <= t1 < t2 < ... < tk <= 254.
// Si traza no es nullptr, registra f actual y mejor de cada iteracion (solo
// para analisis; no cambia la busqueda).
std::vector<int> repararSolucion(const std::vector<int> &inicial,
								 const otsu::TablaOtsu &tabla,
								 const Parametros &parametros,
								 std::vector<io::PuntoTraza> *traza = nullptr);

// Optimo global exacto: entre todas las combinaciones factibles
// 1 <= t1 < ... < tk <= 254 devuelve todas las de mayor f(t) (empates con
// tolerancia 1e-12), en orden lexicografico. Usa programacion dinamica en
// O(k * L^2); el backtracking exhaustivo queda como alternativa en algo.cpp.
std::vector<std::vector<int>>
buscarOptimoExhaustivo(const io::Instancia &instancia,
					   const otsu::TablaOtsu &tabla);

} // namespace algo

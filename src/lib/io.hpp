#pragma once

#include "img.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace io {

// Datos de una instancia SEGMENTACION_MULTIUMBRAL_V1 generada por preprocesar.
// Contiene solo datos de entrada: no incluye umbrales ni soluciones.
struct Instancia {
	std::filesystem::path ruta; // Archivo .txt del que se leyo.
	std::string imagen; // Nombre de la imagen de origen (ej: imagen_01.jpg).
	int ancho = 0;
	int alto = 0;
	int L = 0;					  // Cantidad de niveles de intensidad (256).
	int k = 0;					  // Cantidad de umbrales a encontrar.
	unsigned long long N = 0;	  // Cantidad total de pixeles (ancho * alto).
	img::Histograma histograma{}; // histograma[g]: pixeles con intensidad g.
};

// Lee y valida una instancia. Lanza std::runtime_error si el archivo no se
// puede abrir, si falta algun campo o si los datos son inconsistentes.
Instancia leerInstancia(const std::filesystem::path &ruta);

// Comprueba la consistencia de los datos: L = 256, N = ancho * alto,
// 1 <= k <= 254 y que la suma del histograma sea N.
void validarInstancia(const Instancia &instancia);

// Devuelve los .txt de una carpeta ordenados por nombre (ej:
// data/preprocesado/instancias).
std::vector<std::filesystem::path>
listarInstancias(const std::filesystem::path &carpeta);

// Lee todas las instancias de una carpeta (listarInstancias + leerInstancia).
std::vector<Instancia> cargarInstancias(const std::filesystem::path &carpeta);

// Resultado de resolver una instancia.
struct Resultado {
	unsigned int semilla = 0;
	std::vector<int> inicial;
	double fInicial = 0.0;
	std::vector<int> final;
	double fFinal = 0.0;
	double msInicial = 0.0;	   // Milisegundos de generar la solucion inicial.
	double msReparacion = 0.0; // Milisegundos de repararla.
};

// Imprime por consola el resultado de una instancia (umbrales, f y tiempo).
void imprimirResultado(const Instancia &instancia, const Resultado &resultado);

// Imprime por consola el optimo global de una instancia (umbrales, f y tiempo).
// Optimo global de una instancia.
struct Optimo {
	std::vector<std::vector<int>> soluciones; // Todas las soluciones optimas.
	double f = 0.0;							  // Valor optimo de f(t).
	double segundos = 0.0;					  // Tiempo de la busqueda.
};

void imprimirOptimo(const Instancia &instancia, const Optimo &optimo);

// Guardan un archivo de texto por instancia en <carpeta>/<instancia>.txt
// (ej: data/resultados/soluciones/imagen_01_k2.txt). Crean la carpeta si no
// existe, reemplazan el archivo si ya existe y devuelven la ruta escrita.
// Lanzan std::runtime_error si no se puede escribir.
std::filesystem::path guardarResultado(const Instancia &instancia,
									   const Resultado &resultado,
									   const std::filesystem::path &carpeta);
std::filesystem::path guardarOptimo(const Instancia &instancia,
									const Optimo &optimo,
									const std::filesystem::path &carpeta);

} // namespace io

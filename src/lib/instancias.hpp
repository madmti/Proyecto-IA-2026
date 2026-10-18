#pragma once

#include "imagenes.hpp"

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

} // namespace io

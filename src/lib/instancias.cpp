#include "instancias.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>

using namespace std;

namespace mri {
namespace {
const string CABECERA = "SEGMENTACION_MULTIUMBRAL_V1";

[[noreturn]] void error(const filesystem::path &ruta, const string &mensaje) {
	throw runtime_error("Instancia invalida (" + ruta.string() +
						"): " + mensaje);
}

// Lee un entero no negativo; rechaza signos y valores no numericos.
unsigned long long leerNoNegativo(istream &entrada,
								  const filesystem::path &ruta,
								  const string &campo) {
	long long valor;
	if (!(entrada >> valor))
		error(ruta, "valor faltante o no numerico en '" + campo + "'");
	if (valor < 0)
		error(ruta, "valor negativo en '" + campo + "'");
	return static_cast<unsigned long long>(valor);
}
} // namespace

Instancia leerInstancia(const filesystem::path &ruta) {
	ifstream archivo(ruta);
	if (!archivo.is_open())
		throw runtime_error("No se pudo abrir la instancia: " + ruta.string());

	string token;
	if (!(archivo >> token) || token != CABECERA)
		error(ruta, "se esperaba la cabecera " + CABECERA);

	Instancia instancia;
	instancia.ruta = ruta;
	bool tieneImagen = false, tieneAncho = false, tieneAlto = false;
	bool tieneL = false, tieneK = false, tieneN = false;

	// Campos "clave valor" en cualquier orden, hasta la marca "histograma".
	while (archivo >> token && token != "histograma") {
		if (token == "imagen") {
			if (!(archivo >> instancia.imagen))
				error(ruta, "valor faltante en 'imagen'");
			tieneImagen = true;
		} else if (token == "ancho") {
			instancia.ancho =
				static_cast<int>(leerNoNegativo(archivo, ruta, token));
			tieneAncho = true;
		} else if (token == "alto") {
			instancia.alto =
				static_cast<int>(leerNoNegativo(archivo, ruta, token));
			tieneAlto = true;
		} else if (token == "L") {
			instancia.L =
				static_cast<int>(leerNoNegativo(archivo, ruta, token));
			tieneL = true;
		} else if (token == "k") {
			instancia.k =
				static_cast<int>(leerNoNegativo(archivo, ruta, token));
			tieneK = true;
		} else if (token == "N") {
			instancia.N = leerNoNegativo(archivo, ruta, token);
			tieneN = true;
		} else {
			error(ruta, "campo desconocido '" + token + "'");
		}
	}
	if (token != "histograma")
		error(ruta, "falta la marca 'histograma'");
	if (!(tieneImagen && tieneAncho && tieneAlto && tieneL && tieneK && tieneN))
		error(ruta, "faltan campos (imagen, ancho, alto, L, k o N)");
	if (instancia.L != static_cast<int>(instancia.histograma.size()))
		error(ruta, "L debe ser " + to_string(instancia.histograma.size()));

	for (size_t g = 0; g < instancia.histograma.size(); ++g)
		instancia.histograma[g] =
			leerNoNegativo(archivo, ruta, "histograma[" + to_string(g) + "]");
	if (archivo >> token)
		error(ruta, "datos sobrantes despues del histograma");

	validarInstancia(instancia);
	return instancia;
}

void validarInstancia(const Instancia &instancia) {
	const auto &ruta = instancia.ruta;
	if (instancia.L != static_cast<int>(instancia.histograma.size()))
		error(ruta, "L debe ser " + to_string(instancia.histograma.size()));
	if (instancia.ancho <= 0 || instancia.alto <= 0)
		error(ruta, "dimensiones no positivas");
	if (instancia.N != static_cast<unsigned long long>(instancia.ancho) *
						   static_cast<unsigned long long>(instancia.alto))
		error(ruta, "N debe ser ancho * alto");
	if (instancia.k < 1 || instancia.k > instancia.L - 2)
		error(ruta, "k debe estar entre 1 y " + to_string(instancia.L - 2));

	unsigned long long suma = 0;
	for (auto frecuencia : instancia.histograma)
		suma += frecuencia;
	if (suma != instancia.N)
		error(ruta, "la suma del histograma (" + to_string(suma) +
						") no coincide con N (" + to_string(instancia.N) + ")");
}

vector<filesystem::path> listarInstancias(const filesystem::path &carpeta) {
	if (!filesystem::is_directory(carpeta))
		throw runtime_error("No es una carpeta: " + carpeta.string());

	vector<filesystem::path> rutas;
	for (const auto &entrada : filesystem::directory_iterator(carpeta))
		if (entrada.is_regular_file() && entrada.path().extension() == ".txt")
			rutas.push_back(entrada.path());
	sort(rutas.begin(), rutas.end());
	return rutas;
}

vector<Instancia> cargarInstancias(const filesystem::path &carpeta) {
	vector<Instancia> instancias;
	for (const auto &ruta : listarInstancias(carpeta))
		instancias.push_back(leerInstancia(ruta));
	return instancias;
}

} // namespace mri

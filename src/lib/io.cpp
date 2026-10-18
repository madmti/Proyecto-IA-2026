#include "io.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

namespace io {

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

string formatearUmbrales(const vector<int> &umbrales) {
	ostringstream texto;
	texto << "{";
	for (size_t i = 0; i < umbrales.size(); ++i)
		texto << (i ? ", " : "") << umbrales[i];
	texto << "}";
	return texto.str();
}

string listarUmbrales(const vector<int> &umbrales) {
	ostringstream texto;
	for (size_t i = 0; i < umbrales.size(); ++i)
		texto << (i ? " " : "") << umbrales[i];
	return texto.str();
}

template <typename Escribir>
filesystem::path guardarArchivo(const Instancia &instancia,
								const filesystem::path &carpeta,
								Escribir escribir) {
	filesystem::create_directories(carpeta);
	const filesystem::path ruta = carpeta / instancia.ruta.filename();

	ofstream archivo(ruta);
	if (!archivo.is_open())
		throw runtime_error("No se pudo crear: " + ruta.string());
	archivo << fixed;
	escribir(archivo);
	archivo.close();
	if (archivo.fail())
		throw runtime_error("Error al escribir: " + ruta.string());
	return ruta;
}

void imprimirResultado(const Instancia &instancia, const Resultado &resultado) {
	const string inicial = formatearUmbrales(resultado.inicial);
	const string final = formatearUmbrales(resultado.final);
	const int ancho = static_cast<int>(max(inicial.size(), final.size()));

	cout << "[i] " << instancia.ruta.filename().string()
		 << " (k=" << instancia.k << ")\n"
		 << fixed << setprecision(4) << "    inicial  t=" << left << setw(ancho)
		 << inicial << "  f=" << resultado.fInicial << "\n"
		 << "    final    t=" << left << setw(ancho) << final
		 << "  f=" << resultado.fFinal << "  (" << showpos
		 << resultado.fFinal - resultado.fInicial << noshowpos << ")\n"
		 << setprecision(3) << "    tiempo   inicial=" << resultado.msInicial
		 << " ms  reparacion=" << resultado.msReparacion << " ms\n";
}

void imprimirOptimo(const Instancia &instancia, const Optimo &optimo) {
	cout << "[i] " << instancia.ruta.filename().string()
		 << " (k=" << instancia.k << ")\n"
		 << fixed << setprecision(4) << "    optimo   f=" << optimo.f << "  "
		 << optimo.soluciones.size() << " solucion(es)  " << setprecision(3)
		 << optimo.segundos << " s\n";
	for (const auto &umbrales : optimo.soluciones)
		cout << "             t=" << formatearUmbrales(umbrales) << "\n";
}

filesystem::path guardarResultado(const Instancia &instancia,
								  const Resultado &resultado,
								  const filesystem::path &carpeta) {
	return guardarArchivo(instancia, carpeta, [&](ostream &salida) {
		salida << "SOLUCION_MULTIUMBRAL_V1\n"
			   << "instancia " << instancia.ruta.filename().string() << "\n"
			   << "imagen " << instancia.imagen << "\n"
			   << "k " << instancia.k << "\n"
			   << "semilla " << resultado.semilla << "\n"
			   << setprecision(10) << "f_inicial " << resultado.fInicial << "\n"
			   << "umbrales_inicial " << listarUmbrales(resultado.inicial)
			   << "\n"
			   << "f_final " << resultado.fFinal << "\n"
			   << "umbrales_final " << listarUmbrales(resultado.final) << "\n"
			   << setprecision(6) << "ms_inicial " << resultado.msInicial
			   << "\n"
			   << "ms_reparacion " << resultado.msReparacion << "\n";
	});
}

filesystem::path guardarOptimo(const Instancia &instancia, const Optimo &optimo,
							   const filesystem::path &carpeta) {
	return guardarArchivo(instancia, carpeta, [&](ostream &salida) {
		salida << "OPTIMO_MULTIUMBRAL_V1\n"
			   << "instancia " << instancia.ruta.filename().string() << "\n"
			   << "imagen " << instancia.imagen << "\n"
			   << "k " << instancia.k << "\n"
			   << setprecision(10) << "f_optimo " << optimo.f << "\n"
			   << "n_optimos " << optimo.soluciones.size() << "\n";
		for (const auto &umbrales : optimo.soluciones)
			salida << "umbrales " << listarUmbrales(umbrales) << "\n";
		salida << setprecision(6) << "segundos " << optimo.segundos << "\n";
	});
}

} // namespace io

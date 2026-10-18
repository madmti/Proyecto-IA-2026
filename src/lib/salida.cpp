#include "salida.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

namespace io {
namespace {
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
} // namespace

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

void imprimirOptimo(const Instancia &instancia, const algo::Optimo &optimo) {
	cout << "[i] " << instancia.ruta.filename().string()
		 << " (k=" << instancia.k << ")\n"
		 << fixed << setprecision(4)
		 << "    optimo   t=" << formatearUmbrales(optimo.umbrales)
		 << "  f=" << optimo.f << "  " << setprecision(3) << optimo.segundos
		 << " s\n";
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

filesystem::path guardarOptimo(const Instancia &instancia,
							   const algo::Optimo &optimo,
							   const filesystem::path &carpeta) {
	return guardarArchivo(instancia, carpeta, [&](ostream &salida) {
		salida << "OPTIMO_MULTIUMBRAL_V1\n"
			   << "instancia " << instancia.ruta.filename().string() << "\n"
			   << "imagen " << instancia.imagen << "\n"
			   << "k " << instancia.k << "\n"
			   << setprecision(10) << "f_optimo " << optimo.f << "\n"
			   << "umbrales " << listarUmbrales(optimo.umbrales) << "\n"
			   << setprecision(6) << "segundos " << optimo.segundos << "\n";
	});
}

} // namespace io

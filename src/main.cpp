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
const char *const CARPETA_SOLUCIONES = "data/resultados/soluciones";
const unsigned int SEMILLA = 42;

// Verifica las restricciones sobre los umbrales con assert
void assertRestricciones(const std::vector<int> &umbrales) {
	assert((umbrales.size() == 2 || umbrales.size() == 4 ||
			umbrales.size() == 6) &&
		   "Solo se aceptan 2, 4 o 6 umbrales");
	assert(umbrales.front() > 0 && "El primer umbral debe ser > 0");
	assert(umbrales.back() < 255 && "El ultimo umbral debe ser < 255");
	auto check_order = [&umbrales]() -> bool {
		int last = 0;
		for (const int &um : umbrales) {
			if (last >= um)
				return false;
			last = um;
		}
		return true;
	};
	assert(check_order() &&
		   "Los umbrales deben estar ordenados ascendentemente y deben ser "
		   "distintos entre ellos");
}

// Resuelve una instancia especifica
io::Resultado resolver(const io::Instancia &instancia, unsigned int semilla) {
	io::Resultado resultado;
	resultado.semilla = semilla;

	const auto tabla = otsu::prepararOtsu(instancia.histograma, instancia.N);

	const auto inicio = chrono::steady_clock::now();

	resultado.inicial = algo::generarSolucionInicial(instancia, tabla, semilla);
	assertRestricciones(resultado.inicial);
	resultado.fInicial = otsu::funObj(resultado.inicial, tabla);

	resultado.final = algo::repararSolucion(resultado.inicial, tabla, semilla);
	assertRestricciones(resultado.final);
	resultado.fFinal = otsu::funObj(resultado.final, tabla);

	const chrono::duration<double> transcurrido =
		chrono::steady_clock::now() - inicio;
	resultado.segundos = transcurrido.count();

	return resultado;
}

int main() {
	vector<io::Instancia> instancias;
	try {
		instancias = io::cargarInstancias(CARPETA_INSTANCIAS);
	} catch (const exception &e) {
		cerr << "[e] error leyendo instancia: " << e.what() << "\n";
		return 1;
	}
	cout << "[i] Cargadas " << instancias.size() << " instancias desde "
		 << CARPETA_INSTANCIAS << "\n";

	try {
		for (const auto &instancia : instancias) {
			const auto resultado = resolver(instancia, SEMILLA);
			io::imprimirResultado(instancia, resultado);
			io::guardarResultado(instancia, resultado, CARPETA_SOLUCIONES);
		}
	} catch (const exception &e) {
		cerr << "[e] error guardando solucion: " << e.what() << "\n";
		return 1;
	}
	cout << "[i] Soluciones guardadas en " << CARPETA_SOLUCIONES << "\n";
	return 0;
}

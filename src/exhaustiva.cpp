#include "lib/algo_exh.hpp"
#include "lib/instancias.hpp"
#include "lib/otsu.hpp"
#include "lib/salida.hpp"

#include <exception>
#include <iostream>
#include <vector>

using namespace std;

const char *const CARPETA_INSTANCIAS = "data/preprocesado/instancias";
const char *const CARPETA_OPTIMOS = "data/resultados/optimos";

static double segundosDesde(chrono::steady_clock::time_point inicio) {
	return chrono::duration<double>(chrono::steady_clock::now() - inicio)
		.count();
}

// Calcula el optimo global de una instancia.
algo::Optimo resolver(const io::Instancia &instancia) {
	algo::Optimo optimo;
	const auto tabla = otsu::prepararOtsu(instancia.histograma, instancia.N);
	const auto inicio = chrono::steady_clock::now();

	optimo.soluciones = algo::buscarOptimoExhaustivo(instancia, tabla);

	optimo.segundos = segundosDesde(inicio);

	optimo.f = otsu::funObj(optimo.soluciones.front(), tabla);
	return optimo;
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
			if (instancia.k > 4)
				continue;
			const auto optimo = resolver(instancia);
			io::imprimirOptimo(instancia, optimo);
			io::guardarOptimo(instancia, optimo, CARPETA_OPTIMOS);
		}
	} catch (const exception &e) {
		cerr << "[e] error guardando optimo: " << e.what() << "\n";
		return 1;
	}
	cout << "[i] Optimos guardados en " << CARPETA_OPTIMOS << "\n";
	return 0;
}

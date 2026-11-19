#include "lib/algo.hpp"
#include "lib/io.hpp"
#include "lib/otsu.hpp"

#include <cassert>
#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

const char *const CARPETA_INSTANCIAS = "data/preprocesado/instancias";
const char *const CARPETA_SOLUCIONES = "data/resultados/soluciones";

// Opciones de linea de comandos. Sin argumentos se usa la configuracion base.
struct Opciones {
	algo::Parametros parametros;
	string instancia; // Vacio: se resuelven todas las de CARPETA_INSTANCIAS.
	string salida = CARPETA_SOLUCIONES;
	bool ayuda = false;
};

void imprimirAyuda(const char *programa) {
	const algo::Parametros base;
	cout
		<< "Uso: " << programa << " [opciones]\n\n"
		<< "Resuelve las instancias con Greedy + Tabu Search.\n\n"
		<< "  --T <n>              Tenencia: la lista tabu guarda k + T pares "
		   "(defecto "
		<< base.tenencia << ")\n"
		<< "  --iter <n>           Iteraciones de Tabu Search (defecto "
		<< base.iteraciones << ")\n"
		<< "  --semilla <n>        Semilla aleatoria (defecto " << base.semilla
		<< ")\n"
		<< "  --instancia <ruta>   Resuelve solo esa instancia (defecto: todas "
		   "las de "
		<< CARPETA_INSTANCIAS << ")\n"
		<< "  --salida <carpeta>   Carpeta de las soluciones (defecto "
		<< CARPETA_SOLUCIONES << ")\n"
		<< "  --help               Muestra esta ayuda\n";
}

// Convierte un argumento a entero >= minimo; lanza invalid_argument si no
// es un entero valido.
long long leerEntero(const string &opcion, const string &valor,
					 long long minimo) {
	size_t usados = 0;
	long long n = 0;
	try {
		n = stoll(valor, &usados);
	} catch (const exception &) {
		usados = 0;
	}
	if (usados == 0 || usados != valor.size() || n < minimo)
		throw invalid_argument(opcion +
							   " espera un entero >= " + to_string(minimo) +
							   ", se recibio '" + valor + "'");
	return n;
}

Opciones leerOpciones(int argc, char **argv) {
	Opciones opciones;
	for (int i = 1; i < argc; i++) {
		const string opcion = argv[i];
		if (opcion == "--help" || opcion == "-h") {
			opciones.ayuda = true;
			continue;
		}
		if (i + 1 >= argc)
			throw invalid_argument("falta el valor de " + opcion);
		const string valor = argv[++i];
		if (opcion == "--T")
			opciones.parametros.tenencia =
				static_cast<int>(leerEntero(opcion, valor, 0));
		else if (opcion == "--iter")
			opciones.parametros.iteraciones =
				static_cast<int>(leerEntero(opcion, valor, 1));
		else if (opcion == "--semilla")
			opciones.parametros.semilla =
				static_cast<unsigned int>(leerEntero(opcion, valor, 0));
		else if (opcion == "--instancia")
			opciones.instancia = valor;
		else if (opcion == "--salida")
			opciones.salida = valor;
		else
			throw invalid_argument("opcion desconocida: " + opcion);
	}
	return opciones;
}

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

static double msDesde(chrono::steady_clock::time_point inicio) {
	return chrono::duration<double, milli>(chrono::steady_clock::now() - inicio)
		.count();
}

// Resuelve una instancia especifica
io::Resultado resolver(const io::Instancia &instancia,
					   const algo::Parametros &parametros) {
	io::Resultado resultado;
	resultado.semilla = parametros.semilla;
	resultado.tenencia = parametros.tenencia;
	resultado.iteraciones = parametros.iteraciones;

	const auto tabla = otsu::prepararOtsu(instancia.histograma, instancia.N);

	auto inicio = chrono::steady_clock::now();

	resultado.inicial =
		algo::generarSolucionInicial(instancia, tabla, parametros.semilla);

	resultado.msInicial = msDesde(inicio);
	assertRestricciones(resultado.inicial);

	inicio = chrono::steady_clock::now();

	resultado.final =
		algo::repararSolucion(resultado.inicial, tabla, parametros);

	resultado.msReparacion = msDesde(inicio);
	assertRestricciones(resultado.final);

	resultado.fInicial = otsu::funObj(resultado.inicial, tabla);
	resultado.fFinal = otsu::funObj(resultado.final, tabla);

	return resultado;
}

int main(int argc, char **argv) {
	Opciones opciones;
	try {
		opciones = leerOpciones(argc, argv);
	} catch (const exception &e) {
		cerr << "[e] " << e.what() << "\n";
		imprimirAyuda(argv[0]);
		return 1;
	}
	if (opciones.ayuda) {
		imprimirAyuda(argv[0]);
		return 0;
	}

	vector<io::Instancia> instancias;
	const string origen =
		opciones.instancia.empty() ? CARPETA_INSTANCIAS : opciones.instancia;
	try {
		if (opciones.instancia.empty())
			instancias = io::cargarInstancias(CARPETA_INSTANCIAS);
		else
			instancias = {io::leerInstancia(opciones.instancia)};
	} catch (const exception &e) {
		cerr << "[e] error leyendo instancia: " << e.what() << "\n";
		return 1;
	}
	cout << "[i] Cargadas " << instancias.size() << " instancias desde "
		 << origen << "\n";

	try {
		for (const auto &instancia : instancias) {
			const auto resultado = resolver(instancia, opciones.parametros);
			io::imprimirResultado(instancia, resultado);
			io::guardarResultado(instancia, resultado, opciones.salida);
		}
	} catch (const exception &e) {
		cerr << "[e] error guardando solucion: " << e.what() << "\n";
		return 1;
	}
	cout << "[i] Soluciones guardadas en " << opciones.salida << "\n";
	return 0;
}

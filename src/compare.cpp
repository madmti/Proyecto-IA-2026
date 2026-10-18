#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
namespace fs = filesystem;

const char *const CARPETA_SOLUCIONES = "data/resultados/soluciones";
const char *const CARPETA_OPTIMOS = "data/resultados/optimos";

// Lee un archivo de resultados "clave valor" y devuelve sus pares.
map<string, string> leerClaves(const fs::path &ruta) {
	ifstream archivo(ruta);
	if (!archivo)
		throw runtime_error("no se pudo abrir " + ruta.string());
	map<string, string> claves;
	string linea;
	while (getline(archivo, linea)) {
		const auto sep = linea.find(' ');
		if (sep != string::npos)
			claves.emplace(linea.substr(0, sep), linea.substr(sep + 1));
	}
	return claves;
}

double leerDouble(const map<string, string> &claves, const string &clave,
				  const fs::path &ruta) {
	const auto it = claves.find(clave);
	if (it == claves.end())
		throw runtime_error(ruta.string() + ": falta la clave " + clave);
	return stod(it->second);
}

struct Fila {
	string instancia;
	int k = 0;
	double fInicial = 0.0;
	double fFinal = 0.0;
	optional<double> fOptimo; // Vacio si no hay exhaustiva para la instancia.
};

vector<Fila> cargarFilas() {
	vector<fs::path> rutas;
	for (const auto &e : fs::directory_iterator(CARPETA_SOLUCIONES))
		if (e.path().extension() == ".txt")
			rutas.push_back(e.path());
	sort(rutas.begin(), rutas.end());

	vector<Fila> filas;
	for (const auto &ruta : rutas) {
		const auto sol = leerClaves(ruta);
		Fila fila;
		fila.instancia = ruta.filename().string();
		fila.k = stoi(sol.at("k"));
		fila.fInicial = leerDouble(sol, "f_inicial", ruta);
		fila.fFinal = leerDouble(sol, "f_final", ruta);

		const fs::path rutaOptimo = fs::path(CARPETA_OPTIMOS) / ruta.filename();
		if (fs::exists(rutaOptimo))
			fila.fOptimo =
				leerDouble(leerClaves(rutaOptimo), "f_optimo", rutaOptimo);
		filas.push_back(fila);
	}

	// Primero por k y luego por numero de instancia
	sort(filas.begin(), filas.end(), [](const Fila &a, const Fila &b) {
		return tie(a.k, a.instancia) < tie(b.k, b.instancia);
	});
	return filas;
}

// Imprime la brecha respecto al optimo: diferencia absoluta y relativa (%).
void imprimirBrecha(double f, const optional<double> &optimo) {
	if (!optimo) {
		cout << setw(10) << "-" << setw(10) << "-";
		return;
	}
	const double abs_ = *optimo - f;
	const double rel = *optimo != 0.0 ? 100.0 * abs_ / *optimo : 0.0;
	cout << setw(10) << abs_ << setw(9) << rel << "%";
}

int main() {
	vector<Fila> filas;
	try {
		filas = cargarFilas();
	} catch (const exception &e) {
		cerr << "[e] error leyendo resultados: " << e.what() << "\n";
		return 1;
	}

	cout << fixed << setprecision(4) << left << setw(20) << "instancia" << right
		 << setw(3) << "k" << setw(12) << "f_inicial" << setw(12) << "f_final"
		 << setw(12) << "f_optimo" << setw(10) << "abs_ini" << setw(10)
		 << "rel_ini" << setw(10) << "abs_fin" << setw(10) << "rel_fin"
		 << "\n";
	for (const auto &f : filas) {
		cout << left << setw(20) << f.instancia << right << setw(3) << f.k
			 << setw(12) << f.fInicial << setw(12) << f.fFinal << setw(12);
		if (f.fOptimo)
			cout << *f.fOptimo;
		else
			cout << "-";
		imprimirBrecha(f.fInicial, f.fOptimo);
		imprimirBrecha(f.fFinal, f.fOptimo);
		cout << "\n";
	}
	return 0;
}

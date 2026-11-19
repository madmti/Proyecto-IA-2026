#include "algo.hpp"
#include "otsu.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <random>
#include <tuple>
#include <vector>

namespace algo {

/*=================================================================
					Algoritmo Generador
=================================================================*/

void greedy(std::vector<int> &umbrales, double &facc, int k,
			const otsu::TablaOtsu &tabla, std::mt19937 &gen) {
	int size = static_cast<int>(umbrales.size());
	if (size >= k + 1) {
		facc += otsu::fTerm(tabla, umbrales.back(), 256);
		return;
	}
	int cotainf = umbrales.back() + 1, cota_sup = 254 - k + size;
	double max = -1;
	std::vector<int> candidatos;
	for (int um = cotainf; um <= cota_sup; um++) {
		double aporte = otsu::fTerm(tabla, umbrales.back(), um);

		// Incluir el aporte simulado de toda la masa restante
		aporte += otsu::fTerm(tabla, um, 256);

		if (aporte > max) {
			max = aporte;
			candidatos = {um};
		} else if (aporte == max) {
			candidatos.push_back(um);
		}
	};

	// Desempate
	std::uniform_int_distribution<size_t> dist(0, candidatos.size() - 1);
	umbrales.push_back(candidatos[dist(gen)]);
	greedy(umbrales, facc, k, tabla, gen);
}

std::vector<int> generarSolucionInicial(const io::Instancia &instancia,
										const otsu::TablaOtsu &tabla,
										unsigned int semilla) {
	std::vector<int> umbrales = {0};
	double f = 0.0;
	std::mt19937 gen(semilla);
	greedy(umbrales, f, instancia.k, tabla, gen);
	umbrales.erase(umbrales.begin());
	return umbrales;
}

/*=================================================================
					Algoritmo Reparador
=================================================================*/

double funObjMod(std::vector<int> umbrales, std::tuple<int, int> vecino,
				 const otsu::TablaOtsu &tabla) {
	auto [idx, val] = vecino;
	umbrales[idx] = val;
	return otsu::funObj(umbrales, tabla);
}

std::vector<std::tuple<int, int>> generarVecinos(std::vector<int> &actual,
												 int size) {
	std::vector<std::tuple<int, int>> vecinos;

	auto agregarVecinos = [&actual, &vecinos, &size](int mov) {
		for (int i = 0; i < size; i++) {
			int val = actual[i] + mov;
			if (val < 1 || val > 254)
				continue;
			if (std::find(actual.begin(), actual.end(), val) != actual.end())
				continue;
			std::tuple<int, int> vecino = std::make_tuple(i, val);
			vecinos.push_back(vecino);
		}
	};

	agregarVecinos(1);
	agregarVecinos(-1);

	return vecinos;
}

std::vector<int> tabusearch(const std::vector<int> &inicial,
							const otsu::TablaOtsu &tabla,
							const Parametros &parametros,
							std::vector<io::PuntoTraza> *traza) {
	// T arbitrario, probando nomas me sirvio T = 10 que fue mejor que 2 * size - 1
	int size = static_cast<int>(inicial.size()), T = parametros.tenencia;
	std::vector<int> actual = inicial, mejor = inicial;
	double fmax = otsu::funObj(inicial, tabla);

	std::deque<std::tuple<int, int>> tabu;
	for (int i = 0; i < size; i++)
		tabu.push_back(std::make_tuple(i, inicial[i]));

	// Registro para analisis (no interviene en la busqueda).
	if (traza) {
		traza->reserve(parametros.iteraciones + 1);
		traza->push_back({fmax, fmax});
	}

	for (int i = 0; i < parametros.iteraciones; i++) {
		if (T > 0)
			T -= 1;
		else
			tabu.pop_front();
		auto vecinos = generarVecinos(actual, size);

		std::tuple<int, int> candidato = std::make_tuple(-1, -1),
							 local = std::make_tuple(-1, -1);
		double fcandidato = -1.0, flocal = -1.0;

		for (auto &vecino : vecinos) {
			double fvecino = funObjMod(actual, vecino, tabla);
			if (fvecino > flocal) {
				flocal = fvecino;
				local = vecino;
			}
			if (fvecino <= fcandidato)
				continue;
			auto it = std::find(tabu.begin(), tabu.end(), vecino);
			if (it == tabu.end() || fvecino > fmax) {
				fcandidato = fvecino;
				candidato = vecino;
			}
		}

		// Caso borde donde no hay ningun vecino candidato, elegimos nomas el
		// mejor de los tabus
		if (fcandidato < 0) {
			fcandidato = flocal;
			candidato = local;
		}

		auto [idx, val] = candidato;
		actual[idx] = val;
		tabu.push_back(candidato);

		if (fcandidato > fmax) {
			fmax = fcandidato;
			mejor = actual;
		}

		if (traza)
			traza->push_back({fcandidato, fmax});
	}

	return mejor;
}

std::vector<int> repararSolucion(const std::vector<int> &inicial,
								 const otsu::TablaOtsu &tabla,
								 const Parametros &parametros,
								 std::vector<io::PuntoTraza> *traza) {
	return tabusearch(inicial, tabla, parametros, traza);
}

/*=================================================================
					Algoritmo Completo
=================================================================*/

constexpr double EPS_EMPATE = 1e-12;

void backtracking(std::vector<int> umbrales, int next, double facc, int k,
				  const otsu::TablaOtsu &tabla,
				  std::vector<std::vector<int>> &sols, double &maxf) {
	umbrales.push_back(next);
	int size = static_cast<int>(umbrales.size());
	if (size >= k + 1) {
		facc += otsu::fTerm(tabla, umbrales.back(), 256);
		double diff = facc - maxf;
		if (diff > EPS_EMPATE) {
			umbrales.erase(umbrales.begin());
			sols = {umbrales};
			maxf = facc;
		} else if (std::abs(diff) <= EPS_EMPATE) {
			umbrales.erase(umbrales.begin());
			sols.push_back(umbrales);
		}
		return;
	}

	int cotainf = umbrales.back() + 1, cota_sup = 254 - k + size;
	for (int um = cotainf; um <= cota_sup; um++) {
		double aporte = otsu::fTerm(tabla, umbrales.back(), um);
		backtracking(umbrales, um, facc + aporte, k, tabla, sols, maxf);
	}
};

// Programacion dinamica (Fisher, 1958; Luessi et al., 2009). Como f(t) es una
// suma de terminos que dependen solo de los extremos de cada clase, el mejor
// valor con j umbrales y el ultimo en t solo depende del mejor con j - 1:
//   D[1][t] = fTerm(0, t)
//   D[j][t] = max_{s < t} D[j - 1][s] + fTerm(s, t)
//   f*      = max_t D[k][t] + fTerm(t, 256)
// Cuesta O(k * L^2) en vez de O(L^k). Guarda todos los predecesores empatados
// (tolerancia EPS_EMPATE) para devolver todas las soluciones optimas.
void programacionDinamica(int k, const otsu::TablaOtsu &tabla,
						  std::vector<std::vector<int>> &sols) {
	// Iguala el candidato con el mejor valor actual, como en backtracking.
	auto actualizar = [](double valor, int s, double &mejor,
						 std::vector<int> &empatados) {
		double diff = valor - mejor;
		if (diff > EPS_EMPATE) {
			mejor = valor;
			empatados = {s};
		} else if (std::abs(diff) <= EPS_EMPATE) {
			empatados.push_back(s);
		}
	};

	std::vector<std::vector<double>> D(k + 1, std::vector<double>(256, -1.0));
	std::vector<std::vector<std::vector<int>>> pred(
		k + 1, std::vector<std::vector<int>>(256));

	// El umbral j vive en [j, 254 - k + j] para dejar espacio a los demas.
	for (int t = 1; t <= 254 - k + 1; t++)
		D[1][t] = otsu::fTerm(tabla, 0, t);
	for (int j = 2; j <= k; j++)
		for (int t = j; t <= 254 - k + j; t++)
			for (int s = j - 1; s < t; s++)
				actualizar(D[j - 1][s] + otsu::fTerm(tabla, s, t), s, D[j][t],
						   pred[j][t]);

	double maxf = -1.0;
	std::vector<int> finales;
	for (int t = k; t <= 254; t++)
		actualizar(D[k][t] + otsu::fTerm(tabla, t, 256), t, maxf, finales);

	// Reconstruye hacia atras todas las combinaciones empatadas.
	std::vector<int> umbrales(k);
	std::function<void(int, int)> reconstruir = [&](int j, int t) {
		umbrales[j - 1] = t;
		if (j == 1) {
			sols.push_back(umbrales);
			return;
		}
		for (int s : pred[j][t])
			reconstruir(j - 1, s);
	};
	for (int t : finales)
		reconstruir(k, t);
	std::sort(sols.begin(), sols.end());
}

std::vector<std::vector<int>>
buscarOptimoExhaustivo(const io::Instancia &instancia,
					   const otsu::TablaOtsu &tabla) {

	std::vector<std::vector<int>> soluciones;
	programacionDinamica(instancia.k, tabla, soluciones);
	return soluciones;
}

} // namespace algo

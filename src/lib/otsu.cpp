#include "otsu.hpp"
#include <array>
#include <cassert>
#include <cmath>

namespace otsu {

TablaOtsu prepararOtsu(const img::Histograma &hist, unsigned long long N) {
	std::array<double, 256> accP, accGP;
	unsigned long long acclongP = 0, acclongGP = 0;
	auto L = hist.size();

	for (unsigned long long g = 0; g < L; g++) {
		acclongP += hist[g];
		acclongGP += g * hist[g];
		accP[g] = (double)acclongP / N;
		accGP[g] = (double)acclongGP / N;
	}

	return TablaOtsu{accP, accGP, accGP[L - 1]};
}

double funObj(const std::vector<int> &umbrales, const TablaOtsu &tabla) {
	auto f_term = [&tabla](int a, int b) -> double {
		double w_j = (tabla.acumP[b - 1] - tabla.acumP[a - 1]),
			   GP_j = (tabla.acumGP[b - 1] - tabla.acumGP[a - 1]);
		return w_j > 0 ? w_j * pow(GP_j / w_j - tabla.muT, 2) : 0;
	};

	int b = umbrales.front(), a = umbrales.front();
	double f =
		tabla.acumP[b - 1] > 0
			? tabla.acumP[b - 1] *
				  pow(tabla.acumGP[b - 1] / tabla.acumP[b - 1] - tabla.muT, 2)
			: 0;

	for (long unsigned int j = 1; j < umbrales.size(); j++) {
		int umbral = umbrales[j];
		b = umbral;
		f += f_term(a, b);
		a = umbral;
	}

	f += f_term(a, 256);

	return f;
}

} // namespace otsu

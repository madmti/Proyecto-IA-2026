#include "otsu.hpp"
#include <algorithm>
#include <array>
#include <cassert>

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

double fTerm(const TablaOtsu &tabla, int a, int b) {
	auto acum = [](const std::array<double, 256> &v, int g) {
		return g < 0 ? 0.0 : v[std::min(g, 255)];
	};

	const double w_j = acum(tabla.acumP, b - 1) - acum(tabla.acumP, a - 1);
	const double GP_j = acum(tabla.acumGP, b - 1) - acum(tabla.acumGP, a - 1);
	if (w_j <= 0)
		return 0;

	const double dif = GP_j / w_j - tabla.muT;
	return w_j * dif * dif;
}

double funObj(const std::vector<int> &umbrales, const TablaOtsu &tabla) {
	double f = 0;
	int a = 0;
	for (const int b : umbrales) {
		f += fTerm(tabla, a, b);
		a = b;
	}
	return f + fTerm(tabla, a, 256);
}

} // namespace otsu

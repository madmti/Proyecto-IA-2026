#include "lib/img.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

int main(int argc, char **argv) {
	if (argc < 4) {
		cerr << "Uso: " << argv[0] << " imagen carpeta_salida t1 [t2 ... tk]\n";
		return 1;
	}
	try {
		vector<int> umbrales;
		for (int i = 3; i < argc; ++i) {
			string argumento = argv[i];
			size_t usados = 0;
			int t = stoi(argumento, &usados);
			if (usados != argumento.size())
				throw invalid_argument("Umbral no entero");
			umbrales.push_back(t);
		}
		img::validarUmbrales(umbrales);
		const auto imagen = img::cargarImagen(argv[1]);
		const auto histograma = img::calcularHistograma(imagen);
		const auto resultado = img::segmentar(imagen, umbrales);
		const filesystem::path salida = argv[2];
		img::guardarImagen(resultado.etiquetas, salida / "etiquetas.png");
		img::guardarImagen(resultado.visualizacion, salida / "segmentada.png");
		img::guardarHistograma(histograma, salida / "histograma.csv");
		cout << "Imagen: " << imagen.cols << 'x' << imagen.rows
			 << ", intensidad (0,0): " << img::intensidad(imagen, 0, 0)
			 << ", clases: " << umbrales.size() + 1 << "\nSalida: " << salida
			 << '\n';
	} catch (const exception &e) {
		cerr << "Error: " << e.what() << '\n';
		return 1;
	}
}

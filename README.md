# Segmentación multiumbral de imágenes MRI

Proyecto de Inteligencia Artificial 2026-2 (UTFSM). Dado el histograma de una imagen MRI de 256 × 256 en 256 niveles de gris y un número fijo de umbrales `k` (2, 4 o 6), se buscan los `k` umbrales que maximizan la varianza entre clases del criterio de Otsu.

**Técnica:** Greedy (solución inicial) + Tabú Search (mejora).

**Autor:** Matías Peñaloza

## Requisitos

C++17, Make, pkg-config y OpenCV 4. En Ubuntu/Debian:

```bash
sudo apt install g++ make pkg-config libopencv-dev
```

Para compilar los informes y presentaciones de `tex/` se necesita además `pdflatex`, `bibtex` y el paquete `minted`.

## Uso

```bash
make                  # compila y genera las 30 instancias (10 imágenes × k = 2, 4, 6)
make run              # ejecuta Greedy + Tabú Search sobre todas las instancias
make run-exhaustiva   # calcula el óptimo exacto por búsqueda exhaustiva
make run-compare      # compara los resultados con el óptimo exacto
make help             # lista todos los comandos
```

Los ejecutables quedan en `build/bin/`. La semilla de selección de imágenes se puede cambiar con `make SEMILLA=7`.

## Estructura

| Ruta | Contenido |
| --- | --- |
| `src/main.cpp` | Resuelve cada instancia (Greedy + Tabú Search) y guarda las soluciones. |
| `src/exhaustiva.cpp` | Búsqueda exhaustiva del óptimo global. |
| `src/compare.cpp` | Tabla comparativa entre las soluciones y el óptimo. |
| `src/preprocesar.cpp` | Convierte las imágenes originales en instancias (histograma + `k`). |
| `src/segmentar.cpp` | Aplica umbrales dados por consola a una imagen y guarda la segmentación. |
| `src/lib/` | Algoritmos (`algo`), función objetivo de Otsu (`otsu`), lectura y escritura (`io`) y manejo de imágenes (`img`). |
| `data/original/` | Imágenes MRI originales. |
| `data/preprocesado/` | Imágenes de 256 × 256 e instancias generadas. |
| `data/resultados/` | Soluciones y óptimos exactos generados. |
| `tex/` | Informes y presentaciones (`make pdf`). |

## Formato

Una **instancia** es un `.txt` con la cabecera `SEGMENTACION_MULTIUMBRAL_V1`, el tamaño de la imagen, `k`, `N` (número de píxeles) y las 256 frecuencias del histograma.

Una **solución** es un `.txt` con la cabecera `SOLUCION_MULTIUMBRAL_V1`: semilla, umbrales y valor de la función objetivo inicial y final, y tiempos de cada etapa.

Los umbrales cumplen `1 <= t1 < ... < tk <= 254`. Se forman `k + 1` clases `[0, t1)`, `[t1, t2)`, …, `[tk, 256)`.

## Segmentar una imagen

```bash
./build/bin/segmentar <imagen> <carpeta_salida> <t1> [t2 ... tk]
```

Por ejemplo, `./build/bin/segmentar data/preprocesado/imagenes/imagen_01.jpg salida 50 80` guarda `segmentada.png`, `etiquetas.png` y `histograma.csv` en `salida/`.

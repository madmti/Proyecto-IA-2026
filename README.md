# Segmentación multiumbral de imágenes MRI

Proyecto de Inteligencia Artificial 2026-2 (UTFSM). Dado el histograma de una imagen MRI de 256 × 256 en 256 niveles de gris y un número fijo de umbrales `k` (2, 4 o 6), se buscan los `k` umbrales que maximizan la varianza entre clases del criterio de Otsu.

**Técnica asignada:** Greedy (solución inicial) + Tabú Search (mejora).

**Autor:** Matías Peñaloza

## Problema

Los umbrales `1 <= t1 < ... < tk <= 254` dividen las intensidades en `k + 1` clases `[0, t1)`, `[t1, t2)`, …, `[tk, 256)`. Se maximiza

```
f(t) = Σ_j ω_j (μ_j − μ_T)²
```

donde `ω_j` es la proporción de píxeles de la clase `j`, `μ_j` su intensidad media y `μ_T` la intensidad media de la imagen.

Las imágenes provienen del *Brain Tumor Dataset* de Jun Cheng (Figshare, 2017). Hay 10 imágenes × 3 valores de `k`, lo que da 30 instancias.

## Requisitos

C++17, Make, pkg-config y OpenCV 4 (o 5). En Ubuntu/Debian:

```bash
sudo apt install g++ make pkg-config libopencv-dev
```

OpenCV se usa solo para leer, redimensionar y escribir imágenes. Los algoritmos trabajan únicamente sobre el histograma.

Para compilar los informes y presentaciones de `tex/` se necesitan además `pdflatex`, `bibtex`, el paquete `minted` y Python con `pygments`.

## Uso

```bash
make                  # compila y genera las 30 instancias (10 imágenes × k = 2, 4, 6)
make run              # ejecuta Greedy + Tabú Search sobre todas las instancias
make run-exhaustiva   # calcula el óptimo exacto por búsqueda exhaustiva (solo k <= 4)
make run-compare      # compara los resultados con el óptimo exacto
make clean            # elimina lo compilado
make help             # lista todos los comandos
```

Los ejecutables quedan en `build/bin/` y deben ejecutarse desde la raíz del repositorio, porque las rutas de datos son relativas a ella.

La semilla que elige las 10 imágenes se puede cambiar con `make SEMILLA=7`. Por defecto es 42.

La búsqueda exhaustiva omite `k = 6`, porque su espacio de búsqueda tiene unos 3,5 · 10¹¹ elementos. Con `k = 4` tarda algunos segundos por instancia.

## Estructura

| Ruta | Contenido |
| --- | --- |
| `src/main.cpp` | Resuelve cada instancia (Greedy + Tabú Search) y guarda las soluciones. |
| `src/exhaustiva.cpp` | Búsqueda exhaustiva (backtracking) del óptimo global. |
| `src/compare.cpp` | Tabla comparativa entre las soluciones y el óptimo. |
| `src/preprocesar.cpp` | Selecciona las imágenes, las lleva a 256 × 256 en gris y genera las instancias (histograma + `k`). |
| `src/segmentar.cpp` | Aplica umbrales dados por consola a una imagen y guarda la segmentación. |
| `src/lib/algo.*` | Greedy, Tabú Search y búsqueda exhaustiva. |
| `src/lib/otsu.*` | Función objetivo de Otsu con sumas acumuladas (evaluación en O(k)). |
| `src/lib/io.*` | Lectura de instancias y escritura de soluciones y óptimos. |
| `src/lib/img.*` | Manejo de imágenes e histogramas. |
| `data/original/` | Imágenes MRI originales (versionadas). |
| `data/preprocesado/` | Imágenes de 256 × 256 e instancias generadas (no versionadas). |
| `data/resultados/` | Soluciones (`soluciones/`) y óptimos exactos (`optimos/`) generados (no versionados). |
| `tex/` | Informes y presentaciones del ramo (ver abajo). |

## Formatos

Una **instancia** (`data/preprocesado/instancias/imagen_XX_kK.txt`) tiene la cabecera `SEGMENTACION_MULTIUMBRAL_V1`, seguida de la imagen de origen, el ancho, el alto, `L`, `k`, `N` (número de píxeles) y las 256 frecuencias del histograma.

Una **solución** (`data/resultados/soluciones/`) tiene la cabecera `SOLUCION_MULTIUMBRAL_V1` y contiene la semilla, los umbrales y el valor de `f` de la solución inicial (Greedy) y de la final (Tabú), además del tiempo de cada etapa en ms.

Un **óptimo** (`data/resultados/optimos/`) tiene la cabecera `OPTIMO_MULTIUMBRAL_V1` y contiene `f_optimo`, la cantidad de soluciones óptimas empatadas, sus umbrales y el tiempo en segundos.

Todos los archivos usan líneas `clave valor`.

## Segmentar una imagen

```bash
./build/bin/segmentar <imagen> <carpeta_salida> <t1> [t2 ... tk]
```

Por ejemplo, `./build/bin/segmentar data/preprocesado/imagenes/imagen_01.jpg salida 50 80` guarda `segmentada.png`, `etiquetas.png` y `histograma.csv` en `salida/`.

## Informes y presentaciones

Cada entrega del ramo tiene su carpeta en `tex/` con `main.tex` y `Referencias.bib`. Solo se versionan las fuentes.

| Carpeta | Entrega | Comando |
| --- | --- | --- |
| `tex/0-estado-del-arte/` | Informe 1: estado del arte y modelo matemático | `make pdf-estado-del-arte` |
| `tex/1-avance/` | Presentación del estado de avance | `make pdf-avance` |
| `tex/2-implementacion/` | Informe 2: implementación y resultados | `make pdf-implementacion` |
| `tex/3-final/` | Presentación final | `make pdf-final` |

`make pdf` compila todas las entregas. `make clean-tex` borra los auxiliares de LaTeX y `make distclean-tex` borra además los PDF.

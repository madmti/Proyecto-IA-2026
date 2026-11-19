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
make run-exhaustiva   # calcula el óptimo exacto de las 30 instancias (programación dinámica)
make run-compare      # compara los resultados con el óptimo exacto
make experimentos     # análisis de sensibilidad de T e iteraciones (CSV)
make clean            # elimina lo compilado
make help             # lista todos los comandos
```

Los ejecutables quedan en `build/bin/` y deben ejecutarse desde la raíz del repositorio, porque las rutas de datos son relativas a ella.

La semilla que elige las 10 imágenes se puede cambiar con `make SEMILLA=7`. Por defecto es 42.

El óptimo exacto se calcula por programación dinámica en O(k · L²) (Fisher, 1958; Luessi et al., 2009) y tarda menos de 1 ms por instancia. Esto aprovecha que `f` es una suma de términos que solo dependen de los extremos de cada clase. Antes se usaba un backtracking exhaustivo, que sigue disponible en `algo.cpp`: con `k = 4` tardaba unos 4 s por instancia y con `k = 6` resultaba impracticable (unas 3,5 · 10¹¹ combinaciones).

### Parámetros del algoritmo

`make run` usa la configuración base. Para cambiarla se ejecuta `main` directamente:

```bash
./build/bin/main [opciones]
```

| Opción | Descripción | Defecto |
| --- | --- | --- |
| `--T <n>` | Tenencia: la lista tabú guarda `k + T` pares (umbral, valor). Entero ≥ 0. | `10` |
| `--iter <n>` | Iteraciones de Tabú Search. Entero ≥ 1. | `1000` |
| `--semilla <n>` | Semilla de los generadores aleatorios. | `42` |
| `--instancia <ruta>` | Resuelve solo esa instancia. | todas las de `data/preprocesado/instancias` |
| `--salida <carpeta>` | Carpeta donde se guardan las soluciones. | `data/resultados/soluciones` |
| `--traza <carpeta>` | Guarda la convergencia de Tabú por instancia en `<carpeta>/<instancia>.csv` (`iteracion,f_actual,f_mejor`; la fila 0 es la solución que recibe Tabú). Es solo para análisis: no cambia la búsqueda. | sin traza |
| `--help` | Muestra la ayuda. | |

Por ejemplo, para resolver una instancia con una lista tabú más corta y menos iteraciones, sin sobrescribir las soluciones base:

```bash
./build/bin/main --instancia data/preprocesado/instancias/imagen_10_k6.txt --T 0 --iter 100 --salida data/resultados/pruebas
```

`run-compare` lee `data/resultados/soluciones` por defecto. Para comparar otra carpeta se usa `./build/bin/compare --soluciones <carpeta>`. Con `--csv`, la tabla sale en formato CSV, con los parámetros, las brechas (%) y los tiempos.

### Experimentos

`make experimentos` ejecuta `scripts/experimentos.sh`. El script calcula los óptimos exactos y resuelve las 30 instancias con cada combinación de `T ∈ {0, 2, 5, 10, 15, 20, 30, 40}` e `iteraciones ∈ {50, 100, 200, 500, 1000, 2000}` (tarda unos segundos). Guarda en `data/resultados/experimentos/`:

| Archivo | Contenido |
| --- | --- |
| `sensibilidad.csv` | Una fila por configuración e instancia: `f` inicial, final y óptimo, brechas y tiempos. |
| `resumen.csv` | Una fila por `(T, iteraciones, k)`: óptimos alcanzados, brecha media y máxima, y tiempo medio de Tabú. |
| `entorno.txt` | CPU, memoria, sistema operativo y compilador usados. |
| `soluciones/T<T>_it<iter>/` | Las soluciones de cada configuración. |

Para cambiar la grilla se editan `TENENCIAS` e `ITERACIONES` al inicio del script.

## Estructura

| Ruta | Contenido |
| --- | --- |
| `src/main.cpp` | Resuelve cada instancia (Greedy + Tabú Search) y guarda las soluciones. |
| `src/exhaustiva.cpp` | Calcula el óptimo global exacto de cada instancia (programación dinámica). |
| `src/compare.cpp` | Tabla comparativa (o CSV) entre las soluciones y el óptimo. |
| `src/preprocesar.cpp` | Selecciona las imágenes, las lleva a 256 × 256 en gris y genera las instancias (histograma + `k`). |
| `src/segmentar.cpp` | Aplica umbrales dados por consola a una imagen y guarda la segmentación. |
| `src/lib/algo.*` | Greedy, Tabú Search y óptimo exacto (programación dinámica y backtracking). |
| `src/lib/otsu.*` | Función objetivo de Otsu con sumas acumuladas (evaluación en O(k)). |
| `src/lib/io.*` | Lectura de instancias y escritura de soluciones y óptimos. |
| `src/lib/img.*` | Manejo de imágenes e histogramas. |
| `data/original/` | Imágenes MRI originales (versionadas). |
| `data/preprocesado/` | Imágenes de 256 × 256 e instancias generadas (no versionadas). |
| `data/resultados/` | Soluciones (`soluciones/`), óptimos exactos (`optimos/`) y experimentos (`experimentos/`) generados (no versionados). |
| `scripts/experimentos.sh` | Grilla de parámetros para el análisis de sensibilidad. |
| `tex/` | Informes y presentaciones del ramo (ver abajo). |

## Formatos

Una **instancia** (`data/preprocesado/instancias/imagen_XX_kK.txt`) tiene la cabecera `SEGMENTACION_MULTIUMBRAL_V1`, seguida de la imagen de origen, el ancho, el alto, `L`, `k`, `N` (número de píxeles) y las 256 frecuencias del histograma.

Una **solución** (`data/resultados/soluciones/`) tiene la cabecera `SOLUCION_MULTIUMBRAL_V1` y contiene los parámetros usados (semilla, tenencia e iteraciones), los umbrales y el valor de `f` de la solución inicial (Greedy) y de la final (Tabú), además del tiempo de cada etapa en ms.

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

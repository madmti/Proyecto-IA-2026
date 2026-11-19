#!/usr/bin/env bash
# Analisis de sensibilidad de Tabu Search sobre las 30 instancias.
#
# Uso (desde la raiz del repositorio): make experimentos
#
# Pasos:
#   1. Optimos globales exactos (programacion dinamica).
#   2. Tuning: resuelve cada instancia con cada combinacion de tenencia (T) e
#      iteraciones y la compara contra el optimo. Los tiempos del informe salen
#      de este paso, que corre sin traza.
#   3. Trazas: una corrida por T con el maximo de iteraciones, registrando la
#      convergencia. Como el algoritmo es determinista, la traza de una corrida
#      con menos iteraciones es el comienzo de esta.
#
# Genera en data/resultados/experimentos/:
#   entorno.txt                      hardware y compilador usados
#   tuning/sensibilidad.csv          una fila por (T, iteraciones, instancia)
#   tuning/resumen.csv               una fila por (T, iteraciones, k): optimos
#                                    alcanzados, brecha media y maxima (%) y
#                                    tiempo medio de Tabu (ms)
#   tuning/soluciones/T<T>_it<iter>/ soluciones de cada configuracion
#   trazas/T<T>/<instancia>.csv      iteracion,f_actual,f_mejor
#   trazas/soluciones/T<T>/          soluciones de esas corridas (sus tiempos
#                                    no se usan)
#
# Y copia a tex/2-implementacion/datos/ los CSV que lee el informe (pgfplots):
#   resumen.csv                      el de tuning/
#   totales.csv                      resumen.csv sumado sobre k: una fila por
#                                    (T, iteraciones) con optimos de 30 y
#                                    brecha maxima (%)
#   base.csv                         filas de sensibilidad.csv con la
#                                    configuracion base (T_BASE, IT_BASE)
#   traza_imagen_10_k6_T<T>.csv      convergencia de imagen_10_k6 con T = 10 y 20
set -euo pipefail

BIN=build/bin
SALIDA=data/resultados/experimentos
TENENCIAS=(0 2 5 10 15 20 30 40)
ITERACIONES=(50 100 200 500 1000 2000)
TOLERANCIA=1e-9 # Brecha (%) bajo la cual se considera alcanzado el optimo.
T_BASE=10
IT_BASE=1000
INFORME=tex/2-implementacion/datos

rm -rf "$SALIDA"
mkdir -p "$SALIDA/tuning" "$SALIDA/trazas"

{
	echo "fecha: $(date -Iseconds)"
	echo "cpu: $(lscpu | sed -n 's/^Model name: *//p')"
	echo "nucleos: $(nproc)"
	echo "memoria: $(free -h | awk '/^Mem:/ {print $2}')"
	echo "sistema: $(uname -sr)"
	echo "compilador: $(g++ --version | head -1)"
} >"$SALIDA/entorno.txt"

# 1. Optimos globales.
echo "[i] 1/3 Optimos exactos"
"$BIN/exhaustiva" >/dev/null

# 2. Tuning (sin traza: aqui se miden los tiempos).
echo "[i] 2/3 Tuning: ${#TENENCIAS[@]} valores de T x ${#ITERACIONES[@]} de iteraciones"
CSV="$SALIDA/tuning/sensibilidad.csv"
for T in "${TENENCIAS[@]}"; do
	for it in "${ITERACIONES[@]}"; do
		carpeta="$SALIDA/tuning/soluciones/T${T}_it${it}"
		"$BIN/main" --T "$T" --iter "$it" --salida "$carpeta" >/dev/null
		# Solo la primera configuracion aporta la cabecera del CSV.
		if [[ -f "$CSV" ]]; then
			"$BIN/compare" --soluciones "$carpeta" --csv | tail -n +2 >>"$CSV"
		else
			"$BIN/compare" --soluciones "$carpeta" --csv >"$CSV"
		fi
	done
done

# Columnas de sensibilidad.csv: 1 instancia, 2 k, 3 tenencia, 4 iteraciones,
# 9 brecha_final, 11 ms_reparacion.
awk -F, -v tol="$TOLERANCIA" '
NR > 1 {
	c = $3 "," $4 "," $2
	if (!(c in n)) orden[++m] = c
	n[c]++
	opt[c] += ($9 < tol)
	suma[c] += $9
	if ($9 > max[c]) max[c] = $9
	ms[c] += $11
}
END {
	print "tenencia,iteraciones,k,optimos,instancias,brecha_media,brecha_max,ms_medio"
	for (i = 1; i <= m; i++) {
		c = orden[i]
		printf "%s,%d,%d,%.6f,%.6f,%.6f\n", c, opt[c], n[c], suma[c] / n[c], max[c], ms[c] / n[c]
	}
}' "$CSV" >"$SALIDA/tuning/resumen.csv"

# 3. Trazas de convergencia (una por T, con el maximo de iteraciones).
MAX_ITER=$(printf '%s\n' "${ITERACIONES[@]}" | sort -n | tail -1)
echo "[i] 3/3 Trazas: ${#TENENCIAS[@]} valores de T con $MAX_ITER iteraciones"
for T in "${TENENCIAS[@]}"; do
	"$BIN/main" --T "$T" --iter "$MAX_ITER" \
		--salida "$SALIDA/trazas/soluciones/T${T}" \
		--traza "$SALIDA/trazas/T${T}" >/dev/null
done

# 4. Datos para el informe.
rm -rf "$INFORME"
mkdir -p "$INFORME"
cp "$SALIDA/tuning/resumen.csv" "$INFORME/"
# Columnas de resumen.csv: 1 tenencia, 2 iteraciones, 4 optimos, 5 instancias,
# 7 brecha_max.
awk -F, '
NR > 1 {
	c = $1 "," $2
	if (!(c in n)) orden[++m] = c
	opt[c] += $4
	n[c] += $5
	if ($7 > max[c]) max[c] = $7
}
END {
	print "tenencia,iteraciones,optimos,instancias,brecha_max"
	for (i = 1; i <= m; i++) {
		c = orden[i]
		printf "%s,%d,%d,%.6f\n", c, opt[c], n[c], max[c]
	}
}' "$SALIDA/tuning/resumen.csv" >"$INFORME/totales.csv"
awk -F, -v t="$T_BASE" -v it="$IT_BASE" 'NR == 1 || ($3 == t && $4 == it)' \
	"$CSV" >"$INFORME/base.csv"
for T in 10 20; do
	cp "$SALIDA/trazas/T${T}/imagen_10_k6.csv" "$INFORME/traza_imagen_10_k6_T${T}.csv"
done

echo "[i] Resultados en $SALIDA (entorno.txt, tuning/, trazas/)"
echo "[i] Datos del informe en $INFORME"

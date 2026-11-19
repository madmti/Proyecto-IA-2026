#!/usr/bin/env bash
# Analisis de sensibilidad de Tabu Search: resuelve las 30 instancias para cada
# combinacion de tenencia (T) e iteraciones y compara contra el optimo exacto.
#
# Uso (desde la raiz del repositorio): make experimentos
#
# Genera en data/resultados/experimentos/:
#   sensibilidad.csv  una fila por (T, iteraciones, instancia)
#   resumen.csv       una fila por (T, iteraciones, k): optimos alcanzados,
#                     brecha media y maxima (%) y tiempo medio de Tabu (ms)
#   entorno.txt       hardware y compilador usados
set -euo pipefail

BIN=build/bin
SALIDA=data/resultados/experimentos
TENENCIAS=(0 2 5 10 15 20 30 40)
ITERACIONES=(50 100 200 500 1000 2000)
TOLERANCIA=1e-9 # Brecha (%) bajo la cual se considera alcanzado el optimo.

rm -rf "$SALIDA"
mkdir -p "$SALIDA"

echo "[i] Calculando optimos exactos"
"$BIN/exhaustiva" >/dev/null

{
	echo "fecha: $(date -Iseconds)"
	echo "cpu: $(lscpu | sed -n 's/^Model name: *//p')"
	echo "nucleos: $(nproc)"
	echo "memoria: $(free -h | awk '/^Mem:/ {print $2}')"
	echo "sistema: $(uname -sr)"
	echo "compilador: $(g++ --version | head -1)"
} >"$SALIDA/entorno.txt"

CSV="$SALIDA/sensibilidad.csv"
for T in "${TENENCIAS[@]}"; do
	for it in "${ITERACIONES[@]}"; do
		carpeta="$SALIDA/soluciones/T${T}_it${it}"
		echo "[i] T=$T iteraciones=$it"
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
}' "$CSV" >"$SALIDA/resumen.csv"

echo "[i] Resultados en $SALIDA (sensibilidad.csv, resumen.csv, entorno.txt)"

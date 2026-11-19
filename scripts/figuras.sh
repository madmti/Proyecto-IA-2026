#!/usr/bin/env bash
# Genera las imagenes segmentadas del informe en tex/2-implementacion/figuras/.
#
# Uso (desde la raiz del repositorio): make figuras
#
# Genera:
#   imagen_XX_original.jpg           imagen preprocesada (256 x 256, gris)
#   imagen_XX_kK.png                 segmentacion con los umbrales finales de
#                                    Greedy + Tabu (configuracion base)
#   imagen_10_k6_optimo.png          segmentacion con el optimo exacto, para
#                                    comparar con imagen_10_k6.png
set -euo pipefail

BIN=build/bin
IMAGENES=data/preprocesado/imagenes
SOLUCIONES=data/resultados/soluciones
OPTIMOS=data/resultados/optimos
SALIDA=tex/2-implementacion/figuras
SELECCION=(03 10)
VALORES_K=(2 4 6)

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# Lee los umbrales de una linea "clave t1 t2 ..." de un archivo de resultados.
umbrales() { awk -v clave="$2" '$1 == clave { $1 = ""; print; exit }' "$1"; }

# Segmenta una imagen y copia la visualizacion al destino.
segmentar() {
	local imagen=$1 destino=$2
	shift 2
	"$BIN/segmentar" "$imagen" "$TMP/seg" "$@" >/dev/null
	cp "$TMP/seg/segmentada.png" "$destino"
}

echo "[i] Resolviendo con la configuracion base y calculando optimos"
"$BIN/main" >/dev/null
"$BIN/exhaustiva" >/dev/null

mkdir -p "$SALIDA"
for img in "${SELECCION[@]}"; do
	cp "$IMAGENES/imagen_$img.jpg" "$SALIDA/imagen_${img}_original.jpg"
	for k in "${VALORES_K[@]}"; do
		# shellcheck disable=SC2046 # Los umbrales deben separarse en argumentos.
		segmentar "$IMAGENES/imagen_$img.jpg" "$SALIDA/imagen_${img}_k$k.png" \
			$(umbrales "$SOLUCIONES/imagen_${img}_k$k.txt" umbrales_final)
	done
done

# Caso donde Tabu (T = 10) no alcanza el optimo.
# shellcheck disable=SC2046
segmentar "$IMAGENES/imagen_10.jpg" "$SALIDA/imagen_10_k6_optimo.png" \
	$(umbrales "$OPTIMOS/imagen_10_k6.txt" umbrales)

echo "[i] Figuras en $SALIDA"

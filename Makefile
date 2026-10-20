CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
CPPFLAGS = -Isrc -MMD -MP
OPENCV_PKG := $(shell pkg-config --exists opencv5 && echo opencv5 || echo opencv4)
OPENCV_CFLAGS := $(patsubst -I%,-isystem %,$(shell pkg-config --cflags $(OPENCV_PKG)))
OPENCV_LIBS = -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
DATA_ORIGINAL = data/original
DATA_PREPROCESADO = data/preprocesado
SEMILLA ?= 42

PROGRAMAS_SRC := $(wildcard $(SRC_DIR)/*.cpp)
COMUNES_SRC := $(wildcard $(SRC_DIR)/*/*.cpp $(SRC_DIR)/*/*/*.cpp)

PROGRAMAS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%,$(PROGRAMAS_SRC))
PROGRAMAS_OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(PROGRAMAS_SRC))
COMUNES_OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(COMUNES_SRC))
DEPS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.d,$(PROGRAMAS_SRC) $(COMUNES_SRC))

TEX_DIR = tex
TEX_ENTREGAS = 0-estado-del-arte 1-avance 2-implementacion 3-final
TEX_PDFS := $(foreach e,$(TEX_ENTREGAS),$(TEX_DIR)/$(e)/main.pdf)
PDFLATEX = pdflatex -interaction=nonstopmode -halt-on-error
BIBTEX = bibtex
TEX_AUX = aux log bbl blg out toc nav snm vrb lof lot fls fdb_latexmk synctex.gz

PREPROCESADO_STAMP := $(DATA_PREPROCESADO)/.generado
IMAGENES_ORIGINALES := $(wildcard $(DATA_ORIGINAL)/*.jpg $(DATA_ORIGINAL)/*.jpeg)

.PHONY: all programas preprocesado run run-exhaustiva run-compare clean clean-datos distclean help \
	pdf pdf-estado-del-arte pdf-avance pdf-implementacion pdf-final clean-tex distclean-tex
.SECONDEXPANSION:
.SECONDARY: $(PROGRAMAS_OBJ)

all: programas preprocesado

programas: $(PROGRAMAS)

$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(COMUNES_OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(OPENCV_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OPENCV_CFLAGS) -c $< -o $@

preprocesado: $(PREPROCESADO_STAMP)

$(PREPROCESADO_STAMP): $(SRC_DIR)/preprocesar.cpp $(IMAGENES_ORIGINALES) | $(BIN_DIR)/preprocesar
	$(RM) -r $(DATA_PREPROCESADO)/imagenes $(DATA_PREPROCESADO)/instancias
	./$(BIN_DIR)/preprocesar $(DATA_ORIGINAL) $(DATA_PREPROCESADO) $(SEMILLA)
	@touch $@

run: $(BIN_DIR)/main preprocesado
	@./$(BIN_DIR)/main

run-exhaustiva: $(BIN_DIR)/exhaustiva preprocesado
	@./$(BIN_DIR)/exhaustiva

run-compare: $(BIN_DIR)/compare
	@./$(BIN_DIR)/compare

# Informes y presentaciones (pdflatex + bibtex), una carpeta por entrega.
pdf: $(TEX_PDFS)
pdf-estado-del-arte: $(TEX_DIR)/0-estado-del-arte/main.pdf
pdf-avance: $(TEX_DIR)/1-avance/main.pdf
pdf-implementacion: $(TEX_DIR)/2-implementacion/main.pdf
pdf-final: $(TEX_DIR)/3-final/main.pdf

$(TEX_DIR)/%/main.pdf: $$(wildcard $(TEX_DIR)/$$*/*.tex $(TEX_DIR)/$$*/*.bib)
	cd $(@D) && $(PDFLATEX) main.tex && $(BIBTEX) main && $(PDFLATEX) main.tex && $(PDFLATEX) main.tex

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)

clean-datos:
	$(RM) -r $(DATA_PREPROCESADO)/imagenes $(DATA_PREPROCESADO)/instancias $(PREPROCESADO_STAMP)

clean-tex:
	@for e in $(TEX_ENTREGAS); do for x in $(TEX_AUX); do $(RM) $(TEX_DIR)/$$e/*.$$x; done; done

distclean-tex: clean-tex
	$(RM) $(TEX_PDFS)

distclean: clean clean-datos

help:
	@echo "make                Compila y genera las instancias"
	@echo "make programas      Solo compila"
	@echo "make preprocesado   Solo genera las instancias"
	@echo "make run            Compila y ejecuta main"
	@echo "make run-exhaustiva Compila y ejecuta la busqueda exhaustiva"
	@echo "make run-compare    Compara greedy vs exhaustiva en una tabla"
	@echo "make clean          Elimina lo compilado"
	@echo "make clean-datos    Elimina las instancias generadas"
	@echo "make distclean      clean + clean-datos"
	@echo "make pdf            Compila todos los informes y presentaciones (tex/)"
	@echo "make pdf-estado-del-arte | pdf-avance | pdf-implementacion | pdf-final"
	@echo "                    Compila una entrega por separado"
	@echo "make clean-tex      Elimina auxiliares de LaTeX (conserva los PDF)"
	@echo "make distclean-tex  clean-tex + elimina los PDF"
	@echo "make help           Muestra esta ayuda"

-include $(DEPS)

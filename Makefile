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

PREPROCESADO_STAMP := $(DATA_PREPROCESADO)/.generado
IMAGENES_ORIGINALES := $(wildcard $(DATA_ORIGINAL)/*.jpg $(DATA_ORIGINAL)/*.jpeg)

.PHONY: all programas preprocesado run clean clean-datos distclean help
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

# Compila y ejecuta main.
run: $(BIN_DIR)/main preprocesado
	@./$(BIN_DIR)/main

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)

clean-datos:
	$(RM) -r $(DATA_PREPROCESADO)/imagenes $(DATA_PREPROCESADO)/instancias $(PREPROCESADO_STAMP)

distclean: clean clean-datos

help:
	@echo "make                Compila y genera las instancias"
	@echo "make programas      Solo compila"
	@echo "make preprocesado   Solo genera las instancias"
	@echo "make run            Compila y ejecuta main"
	@echo "make clean          Elimina lo compilado"
	@echo "make clean-datos    Elimina las instancias generadas"
	@echo "make distclean      clean + clean-datos"
	@echo "make help           Muestra esta ayuda"

-include $(DEPS)

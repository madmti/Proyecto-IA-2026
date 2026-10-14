CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
OPENCV_PKG := $(shell pkg-config --exists opencv5 && echo opencv5 || echo opencv4)
OPENCV = $(shell pkg-config --cflags $(OPENCV_PKG)) -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

.PHONY: all clean

all: preprocesar segmentar

segmentar: segmentar.cpp imagenes.cpp imagenes.hpp
	$(CXX) $(CXXFLAGS) segmentar.cpp imagenes.cpp -o segmentar $(OPENCV)

preprocesar: preprocesar.cpp
	$(CXX) $(CXXFLAGS) preprocesar.cpp -o preprocesar $(OPENCV)

clean:
	$(RM) preprocesar segmentar *.o *.d

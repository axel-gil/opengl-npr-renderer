platform=$(shell uname -o)

ifeq ($(platform),Darwin)
CC = clang++
CXX = clang++
else
CC = g++
endif

SRC = image.cc  image_io.cc matrix4.cc transformation.cc program.cc \
	  tiny_obj_loader.cc camera.cc
OBJ = $(SRC:.cc=.o)

CXXFLAGS += -Wall -Wextra -O3 -g -std=c++11
CXXFLAGS += -m64 -march=native

ifeq ($(CC),clang++)
# CXXFLAGS += -Rpass=loop-vectorize \
# 			 -Rpass-missed=loop-vectorize \
# 			 -Rpass-analysis=loop-vectorize
else
CXXFLAGS += -fopt-info-vec-optimized \
			 -fopt-info-vec-missed \
			 -ftree-vectorize
endif

ifeq ($(platform),Darwin)
LDLIBS = -framework OpenGL \
		 -framework Cocoa \
		 -framework IOKit \
		 -lglfw \
		 -lGLEW
else
LDLIBS = -lGL  -lGLEW -lglut -lpthread
endif

# roman instructions

ifeq ($(platform),Darwin)
CPPFLAGS += -I/opt/homebrew/opt/glfw/include \
		   	-I/opt/homebrew/opt/glew/include
LDFLAGS += -L/opt/homebrew/lib \
		   -L/opt/homebrew/opt/glew/lib \
           -L/opt/homebrew/opt/glfw/lib
else
LIBS=glfw3 gl glew
CPPFLAGS += $(shell pkg-config --cflags $(LIBS) )
LDFLAGS += $(shell pkg-config --libs $(LIBS) )
endif

all: main

main: $(OBJ)

clean:
	$(RM) $(OBJ) main

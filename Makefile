platform=$(shell uname -o)

ifeq ($(platform),Darwin)
CC = clang++
CXX = clang++
else
CC = g++
endif

SRC = main.cc \
	  src/image.cc \
	  src/image_io.cc \
	  src/matrix4.cc \
	  src/transformation.cc \
	  src/program.cc \
	  src/tiny_obj_loader.cc \
	  src/camera.cc \
	  src/init_gl.cc 

OBJ = $(SRC:.cc=.o)

CXXFLAGS += -Wall -Wextra -O3 -g -std=c++11
CXXFLAGS += -m64 -march=native

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

CPPFLAGS += -Isrc/

all: main

main: $(OBJ)

clean:
	$(RM) $(OBJ) main

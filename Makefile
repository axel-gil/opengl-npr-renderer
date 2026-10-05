platform=$(shell uname -o)

ifeq ($(platform),Darwin)
CC = clang++
CXX = clang++
CXXFLAGS += -fcolor-diagnostics
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
	  src/init_gl.cc \
	  src/vector3.cc \
	  src/object.cc \
	  src/billboard.cc \
	  src/color.cc \
	  src/particle.cc \
	  src/fire.cc \
		src/utils.cc

OBJ = $(SRC:.cc=.o)

CXXFLAGS += -Wall -Wextra -g -std=c++20
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
		   	-I/opt/homebrew/opt/glew/include \
		   	-DGL_SILENCE_DEPRECATION
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

# The forest mesh is a 213 MB ASCII OBJ, too large to version. It is published
# as a release asset (gzipped, ~54 MB) and fetched on demand.
GH_REPO   ?= axel-gil/opengl-npr-renderer
ASSET_TAG ?= v1.0
SCENE     := objects/scene.obj
SCENE_URL := https://github.com/$(GH_REPO)/releases/download/$(ASSET_TAG)/scene.obj.gz

assets: $(SCENE)

$(SCENE):
	@echo "==> Fetching scene mesh (~54 MB compressed)"
	@curl -fL --progress-bar "$(SCENE_URL)" -o $(SCENE).gz
	@gunzip -f $(SCENE).gz
	@echo "==> Scene ready: $(SCENE)"

run: main $(SCENE)
	./main $(SCENE)

clean:
	$(RM) $(OBJ) main

# Removes the downloaded scene as well
distclean: clean
	$(RM) $(SCENE) $(SCENE).gz

.PHONY: all assets run clean distclean

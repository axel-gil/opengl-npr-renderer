#************************************************
#*                                              *
#*             (c) 2017 J. FABRIZIO             *
#*                                              *
#*                               LRDE EPITA     *
#*                                              *
#************************************************

platform=$(shell uname -o)

ifeq ($(platform),Darwin)
CC = clang++
CXX = clang++
else
CC = g++
endif

SRCS = image.cc  image_io.cc matrix4.cc transformation.cc
HXX_FILES = image.hh  image_io.hh
HXX_FILES += object_vbo.hh
OBJ = $(SRCS:.cc=.o)

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

# MAIN_FILE = main.cc
# DIST = main
# 
# SKEL_DIST_DIR = pogl_skel_tp
# SKEL_FILES = $(SRCS) $(HXX_FILES) $(MAIN_FILE) Makefile vertex.glsl fragment.glsl texture.tga lighting.tga normalmap.tga
# 
# 
# #For gcc 4.9
# #CXXFLAGS+=-fdiagnostics-color=auto
# export GCC_COLORS=1
# 
# define color
#     if test -n "${TERM}" ; then\
# 	if test `tput colors` -gt 0 ; then \
# 	    tput setaf $(1); \
#         fi;\
#     fi
# endef
# 
# define default_color
#     if test -n "${TERM}" ; then\
# 	if test `tput colors` -gt 0 ; then  tput sgr0 ; fi; \
#     fi
# endef
# 
# 
# all: post-build
# 
# pre-build:
# 	@$(call color,4)
# 	@echo "******** Starting Compilation ************"
# 	@$(call default_color)
# 
# post-build:
# 	@make --no-print-directory main-build ; \
# 	sta=$$?;	  \
# 	$(call color,4); \
# 	echo "*********** End Compilation **************"; \
# 	$(call default_color); \
# 	exit $$sta;
# 
# main-build: pre-build build
# 
# build: $(OBJ)
# 	$(CC) $(MAIN_FILE) -o $(DIST) $(OBJ) $(CXX_FLAGS) $(LDXX_FLAGS)
# 
# 
# %.o: %.cc %.hh
# 	@$(call color,2)
# 	@echo "[$@] $(CXX_FLAGS)"
# 	@$(call default_color)
# 	@$(CC) -c -o $@ $< $(CXX_FLAGS) ; \
# 	sta=$$?;	  \
# 	if [ $$sta -eq 0 ]; then  \
# 	  $(call color,2) ; \
# 	  echo "[$@ succes]" ; \
# 	  $(call default_color) ; \
# 	else  \
# 	  $(call color,1) ; \
# 	  echo "[$@ failure]" ; \
# 	  $(call default_color) ; \
# 	fi ;\
# 	exit $$sta
# 
# .PHONY: all clean pre-build post-build main-build build skel
# 
# clean:
# 	rm -f $(OBJ)
# 	rm -f $(DIST)
# 	rm -rf $(SKEL_DIST_DIR).tar.bz2
# 
# 
# skel:
# 	rm -rf $(SKEL_DIST_DIR)
# 	mkdir $(SKEL_DIST_DIR)
# 	cp $(SKEL_FILES) $(SKEL_DIST_DIR)
# 	tar -cjvf $(SKEL_DIST_DIR).tar.bz2 $(SKEL_DIST_DIR)
# 	rm -rf $(SKEL_DIST_DIR)

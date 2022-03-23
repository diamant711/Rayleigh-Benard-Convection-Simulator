SOURCES = $(SRC_DIR)/main.cpp                            \
					$(SRC_DIR)/RayBenConvection.hpp                \
					$(SRC_DIR)/Connection.hpp                      \
					$(SRC_DIR)/Server.hpp                          \
					$(SRC_DIR)/TCPServer.hpp											 \
					$(SRC_DIR)/WebServer.hpp											 \
					$(SRC_DIR)/WebPage.hpp

OBJECTS = $(OBJ_DIR)/main.o

STATIC_LIB = $(LIB_DIR)/libraylib.a                      \
						 $(LIB_DIR)/libcolamd.a                      \
						 $(LIB_DIR)/libsuitesparseconfig.a

DINAMIC_LIB = -lm                                        \
							-lpthread                                  \
							-ldl

PRJ_DIR = $(shell pwd)
SRC_DIR = src
INC_DIR = inc
LIB_DIR = lib
OBJ_DIR = obj
REPOS_DIR = repos
CXX = g++
CXXFLAGS = -O3 -I$(INC_DIR)/ -Wextra -std=c++11 -pg -g#altro
LDFLAGS = $(STATIC_LIB) $(DINAMIC_LIB) -fopenmp#altro
X_NAME = CaosTester
RAYLIB_REPO = https://github.com/raysan5/raylib.git
RAYLIB_LIB_FILE = $(PRJ_DIR)/$(LIB_DIR)/libraylib.a
EMSDK_REPO = https://github.com/emscripten-core/emsdk.git
EMSDK_SDK_COMPILER = $(PRJ_DIR)/$(REPOS_DIR)/emsdk/upstream/emscripten/emcc
EMSDK_SDK_ARCHIVER = $(PRJ_DIR)/$(REPOS_DIR)/emsdk/upstream/emscripten/emar

all: env $(X_NAME)

$(X_NAME): $(SOURCES) $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(X_NAME)

$(OBJ_DIR)/%.o:: $(SRC_DIR)/%.cpp
	$(CXX) $(<D)/$(<F) $(CXXFLAGS) -c -o $(@D)/$(@F)

env: ;
	mkdir -p $(OBJ_DIR) $(REPOS_DIR)
	@if [ ! -e "$(EMSDK_SDK_COMPILER)" ] || [ ! -e "$(EMSDK_SDK_ARCHIVER)" ] ; then\
		cd $(REPOS_DIR);\
		git clone $(EMSDK_REPO) emsdk;\
		cd emsdk/;\
		./emsdk install latest;\
		./emsdk activate latest;\
		cd $(PRJ_DIR);\
	fi
	@if [ ! -e "$(RAYLIB_LIB_FILE)" ] ; then\
		cd $(REPOS_DIR);\
		git clone $(RAYLIB_REPO) raylib;\
		cd raylib/src/;\
		$(EMSDK_SDK_COMPILER) -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c utils.c -Os -Wall -DPLATFORM_WEB;\
		$(EMSDK_SDK_COMPILER) -c raudio.c -Os -Wall -DPLATFORM_WEB;\
		$(EMSDK_SDK_ARCHIVER) rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o;\
		cp libraylib.a $(PRJ_DIR)/$(LIB_DIR)/;\
		cd $(PRJ_DIR);\
	fi
	@echo "All enviroment component ready"

clean: ;
	cd $(OBJ_DIR)
	rm -f $(OBJECTS)
	cd $(PRJ_DIR)
	rm -f $(X_NAME)

purge: ;
	make clean
	rm -rf $(REPOS_DIR)/* 

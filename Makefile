# Machine dependent variables
TMP_DIR = /temporanea

# Machine indipendent variables
HEADERS =	$(SRC_DIR)/RayBenConvection.hpp                \
					$(SRC_DIR)/Connection.hpp                      \
					$(SRC_DIR)/Server.hpp                          \
					$(SRC_DIR)/TCPServer.hpp											 \
					$(SRC_DIR)/WebServer.hpp											 \
					$(SRC_DIR)/WebSocketServer.hpp								 \
					$(SRC_DIR)/WebPage.hpp

OBJECTS = $(OBJ_DIR)/main.o

STATIC_LIB = $(LIB_DIR)/libcolamd.a                      \
						 $(LIB_DIR)/libsuitesparseconfig.a
						
DINAMIC_LIB = -lm                                        \
              -lpthread                                  \
              -ldl                                       \
							-lboost_system                             \
							-lssl                                      \
							-lcrypto

PRJ_DIR = $(shell pwd)
SRC_DIR = src
INC_DIR = inc
LIB_DIR = lib
OBJ_DIR = obj
DOC_DIR = doc
REPOS_DIR = $(TMP_DIR)/rbcs_tmp_repos_$(USER).d
CXX = g++
COMMON_FLAGS = -fopenmp -pg -g -fno-math-errno -march=native -DNDEBUG
CXXFLAGS = -O3 -I$(INC_DIR)/ -Wall -Wextra -std=c++11 $(COMMON_FLAGS)
LDFLAGS = $(STATIC_LIB) $(DINAMIC_LIB) $(COMMON_FLAGS)
X_NAME = Rayleigh-Benard-Convection-Simulator
RAYLIB_REPO = https://github.com/raysan5/raylib.git
RAYLIB_LIB_FILE = $(PRJ_DIR)/$(LIB_DIR)/libraylib.a
EMSDK_REPO = https://github.com/emscripten-core/emsdk.git
EMSDK_SDK_COMPILER = $(REPOS_DIR)/emsdk/upstream/emscripten/emcc
EMSDK_SDK_ARCHIVER = $(REPOS_DIR)/emsdk/upstream/emscripten/emar

# Recipes
all: env doxydoc $(X_NAME) ;
	@echo "Build done!"

$(X_NAME): $(OBJECTS)
	@echo "LD      $@"
	@$(CXX) $(OBJECTS) $(LDFLAGS) -o $(X_NAME)

$(OBJ_DIR)/%.o:: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "CXX     $(@F)"
	@$(CXX) $(<D)/$(<F) $(CXXFLAGS) -c -o $(@D)/$(@F)

raylib: src/raylib_page.c inc/temperature_matrix.h inc/jet.h
	@echo "EMCC    raylib.html raylib.js raylib.wasm"
	@$(EMSDK_SDK_COMPILER) $< lib/libraylib.a -o $@.html -DPLATFORM_WEB -Os -Wall \
		-s USE_GLFW=3 --shell-file /temporanea/rbcs_tmp_repos_$(USER).d/emsdk/upstream/emscripten/src/shell.html\
		-s TOTAL_MEMORY=33554432
	@mv raylib.* cnt/

env: ;
	@if [ ! -e $(OBJ_DIR) ] || [ ! -d $(OBJ_DIR) ] ; then\
		echo "Making objects dir";\
		mkdir -p $(OBJ_DIR);\
	fi
	@if [ ! -e $(REPOS_DIR) ] || [ ! -d $(REPOS_DIR) ] ; then\
		echo "Making temporary dependencies dir";\
		mkdir -p $(REPOS_DIR);\
	fi
	@if [ ! -e $(DOC_DIR) ] || [ ! -d $(DOC_DIR) ] ; then\
		echo "Making doxygen documentation dir";\
		mkdir -p $(DOC_DIR);\
	fi
	@if [ ! -e "$(EMSDK_SDK_COMPILER)" ] || [ ! -e "$(EMSDK_SDK_ARCHIVER)" ] ; then\
		cd $(REPOS_DIR);\
		git clone $(EMSDK_REPO) emsdk;\
		cd emsdk/;\
		echo "Making EMSDK dependencies";\
		./emsdk install latest >/dev/null;\
		./emsdk activate latest >/dev/null;\
		cd $(PRJ_DIR);\
	fi
	@if [ ! -e "$(RAYLIB_LIB_FILE)" ] ; then\
		cd $(REPOS_DIR);\
		git clone $(RAYLIB_REPO) raylib;\
		echo "Making RAYLIB (for web) dependencies";\
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

doxydoc: ;
	@if [ ! -e $(DOC_DIR)/html ] || [ ! -e $(DOC_DIR)/latex ] ; then\
		echo "Making doxygen documentation...";\
		doxygen config-file;\
	fi
	@echo "Doxygen documentation done!"

clean: ;
	@echo "Cleaning compilation files..."
	@cd $(OBJ_DIR)
	rm -f $(OBJECTS)
	@cd $(PRJ_DIR)
	rm -f $(X_NAME)
	rm -f raylib.html raylib.js raylib.wasm gmon.out
	rm -rf $(DOC_DIR)/html $(DOC_DIR)/latex

purge: ;
	@make clean
	@echo "Cleaning dependencies..."
	rm -rf $(REPOS_DIR)
	rm -f lib/libraylib.a

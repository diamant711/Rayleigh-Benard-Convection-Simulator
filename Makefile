PRJ_DIR = $(shell pwd)
LIB_DIR = lib/
RAYLIB_REPO = https://github.com/raysan5/raylib.git
RAYLIB_LIB_FILE = $(PRJ_DIR)/lib/raylib/src/libraylib.a
EMSDK_REPO = https://github.com/emscripten-core/emsdk.git
EMSDK_SDK_COMPILER = $(PRJ_DIR)/lib/emsdk/upstream/emscripten/emcc
EMSDK_SDK_ARCHIVER = $(PRJ_DIR)/lib/emsdk/upstream/emscripten/emar

env:
	@if [ ! -e "$(EMSDK_SDK_COMPILER)" ] && [ ! -e "$(EMSDK_SDK_ARCHIVER)" ] ; then\
		cd $(LIB_DIR);\
		git clone $(EMSDK_REPO) emsdk;\
		cd emsdk/;\
		./emsdk install latest;\
		./emsdk activate latest;\
		cd $(PRJ_DIR);\
	fi
	@if [ ! -e "$(RAYLIB_LIB_FILE)" ] ; then\
		cd $(LIB_DIR);\
		git clone $(RAYLIB_REPO) raylib;\
		cd raylib/;\
		cd src/;\
		$(EMSDK_SDK_COMPILER) -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2;\
		$(EMSDK_SDK_COMPILER) -c utils.c -Os -Wall -DPLATFORM_WEB;\
		$(EMSDK_SDK_COMPILER) -c raudio.c -Os -Wall -DPLATFORM_WEB;\
		$(EMSDK_SDK_ARCHIVER) rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o;\
		cd $(PRJ_DIR);\
	fi
	@echo "All enviroment component ready"

clean:
	#rm obj & exe

purge:
	make clean
	rm -rf ./lib/*

CXX = g++
CXXFLAGS = -O3 #altro
CC = gcc
CFLAGS = -O3 #altro
LDFLAGS = -lm #altro
PRJ_DIR = $(shell pwd)
SRC_DIR = src/
INC_DIR = inc/
LIB_DIR = lib/
OBJ_DIR = obj/
EMSDK_REPO = https://github.com/emscripten-core/emsdk.git
EMSDK_SDK_COMPILER = $(PRJ_DIR)/tool/emcc
EMSDK_SDK_ARCHIVER = $(PRJ_DIR)/tool/emar
X_NAME = CaosTester_server

all: $(SRC_DIR)/{$(SOURCES_LIST)}

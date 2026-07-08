SRC := $(shell find src -name "*.cpp")
CC = g++
LANG_STD = -std=c++17
FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs/"
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3


build:
	$(CC) $(FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC) $(LIBS) -o gameengine

run:
	./gameengine

clean:
	rm gameengine

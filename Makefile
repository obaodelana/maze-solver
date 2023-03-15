SRC := $(wildcard src/*.cpp)
INCLUDES := $(shell pkg-config --cflags --libs raylib)
TARGET = maze-solver

run: $(SRC)
	g++ $(SRC) $(INCLUDES) -o $(TARGET) -std=c++11
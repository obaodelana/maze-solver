SRC = src/main.cpp src/generator.cpp src/solver.cpp
TARGET = maze-solver

run: $(SRC)
	g++ $(SRC) `pkg-config --cflags --libs raylib` -o $(TARGET)
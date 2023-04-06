OBJS := $(patsubst src/%.cpp, out/%.o, $(wildcard src/*.cpp))
TARGET = out/maze-solver

# $< is the first prequisite
# $@ is the target name
# Compile
out/%.o: src/%.cpp
	@mkdir -p out
	g++ -c $< $(shell pkg-config --cflags raylib) -o $@ -std=c++11

# Link
build: $(OBJS)
	g++ $^ $(shell pkg-config --libs raylib) -o $(TARGET) 

clean:
	@$(RM) out/*
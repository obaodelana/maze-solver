SRC := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, out/%.o, $(SRC))
RAYLIB_SRC = $(HOME)/dev/libs/raylib/src
TARGET = maze-solver

# $< is the first prequisite
# $@ is the target name
# Compile
out/%.o: src/%.cpp
	@mkdir -p out
	g++ -c $< $(shell pkg-config --cflags raylib) -o $@ -std=c++11

# Link (desktop)
build: $(OBJS)
	g++ $^ $(shell pkg-config --libs raylib) -o $(TARGET) 

# Web
web: $(SRC) shell.html
	@mkdir -p web
	emcc -o web/$(TARGET).html $(SRC) -Os $(RAYLIB_SRC)/libraylib.a -I.\
		$(shell pkg-config --cflags --libs raylib) --shell-file shell.html\
		-s USE_GLFW=3 -DPLATFORM_WEB

clean:
	@$(RM) $(TARGET)
	@$(RM) out/* web/*
# --- Setup ---
CC = gcc
EMCC = emcc
PROJECT_NAME = ./bin/Debug/Primos
SRC = src/main.c

# --- Paths ---
INCLUDE_PATHS = -Isrc -Ilib -Iinclude -Ibuild/external/raylib-master/src

# --- Linux Config ---
# We REMOVE -L. so it doesn't find the web .a file
LIBS_LINUX = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# --- Web Config ---
# We point directly to the specific web library file
LIB_WEB = ./lib/libraylib_web.a
WEB_FLAGS = -DPLATFORM_WEB -s USE_GLFW=3 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 --shell-file shell.html

# --- Targets ---

# Build for Linux
all:
	# $(CC) $(SRC) -o $(PROJECT_NAME) -Os -Wall -g $(INCLUDE_PATHS) $(LIBS_LINUX)
	$(CC) $(SRC) -o $(PROJECT_NAME) -O0 -Wall -g $(INCLUDE_PATHS) $(LIBS_LINUX)

# Build for Web
web:
	$(EMCC) $(SRC) $(LIB_WEB) -o index.html -Os -Wall $(INCLUDE_PATHS) $(WEB_FLAGS)

clean:
	rm -f $(PROJECT_NAME) index.html index.js index.wasm index.data

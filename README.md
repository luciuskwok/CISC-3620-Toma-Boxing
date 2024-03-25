# Toma Boxing (CISC 3620 Midterm Project)

## Building
- Download and install Emscripten from <https://emscripten.org/docs/getting_started/downloads.html>
- `cd` into the emsdk folder
- `source ./emsdk_env.sh` to add the paths

## Building and embedding assets
- `emcc src/*.c -o main.js -sUSE_SDL=2 -sUSE_SDL_MIXER=2 --embed-file assets`

## Historical build commands
These are kept for historical reasons, and should not be used.

### Building "Hello world"
- Basic build: `emcc main.c -o index.html`
- Custom HTML: `emcc main.c -o index.html -O3 --shell-file templates/shell_minimal.html`
- Build JS without changing HTML: `emcc main.c -o main.js -O3`

### Building with SDL2
- `emcc -c main.c -o main.o -s USE_SDL=2`
- `emcc main.o -o ../main.js -s USE_SDL=2`
- `emcc main.c -o ../main.js -s USE_SDL=2`

### Building multiple files
- `cd src`
- `emcc *.c -o ../main.js -sUSE_SDL=2 -sUSE_SDL_MIXER=2`

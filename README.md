# Toma Boxing (CISC 3620 Midterm Project)

This project is for my _CISC 3620 Computer Graphics_ course, and is the midterm project which shows what I've learned in this course so far.

## Building
- Download and install Emscripten from <https://emscripten.org/docs/getting_started/downloads.html>
- `cd` into the emsdk folder
- `source ./emsdk_env.sh` to add the paths

## Building and embedding assets
- `emcc src/*.c -o main.js -sUSE_SDL=2 -sUSE_SDL_MIXER=2 --embed-file assets`
- To include SDL_image: `--use-port=sdl2_image:formats=bmp,png,xpm,jpg`

# Toma Boxing (CISC 3620 Midterm Project)

## Building
- Download and install Emscripten from <https://emscripten.org/docs/getting_started/downloads.html>
- `cd` into the emsdk folder
- `source ./emsdk_env.sh` to add the paths
- Basic build: `emcc main.c -o main.html`
- Custom HTML: `emcc -o main.html main.c -O3 --shell-file _templates/shell_minimal.html`

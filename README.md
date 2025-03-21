# Scoundrel

The classic solitaire card game adventuring rogue-like.

The web version of this game is available here: https://benjamin-t-brown.github.io/scoundrel/

Don't know how to play? Instructions (English) are here: https://benjamin-t-brown.github.io/scoundrel/instructions/instructions.en.html

## ![image](https://github.com/user-attachments/assets/ba5bc38f-a0d4-4410-a22e-c1ce5bbc61f4)

## ![image](https://github.com/user-attachments/assets/d33a161a-bbad-4bdf-992d-c63011a776cf)

Keyboard controls: arrow keys=move cursor, x=confirm, z=back
Gamepad controls: dpad=move cursor, a=confirm, b=back

## Development

Development Requirements

- GCC/Clang
- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_mixer

For Building MiyooA30 Executable

- Docker

For Building WASM Executable

- Emscripten

This program is built with the Makefile in the src directory.

```
cd src
make -j8
make run
```

### Ubuntu

```
apt install\
 build-essential\
 make\
 clangd-17\
 clang-format\
 libsdl2-ttf-dev\
 libsdl2-image-dev\
 libsdl2-mixer-dev\
 libsdl2-gfx-dev -y
```

### Mac M1^ (brew)

```
brew install gcc@13
brew install brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf

# additional development tools
python3 -m pip install --upgrade setuptools
python3 -m pip install --upgrade pip
```

### Mingw64 (Windows)

Development on Windows is done through MSYS2. Assuming you're using mingw64 (other distribs like ucrt or clang should work fine):

```
pacman -S base-devel\
 gcc\
 mingw64/mingw-w64-x86_64-SDL2\
 mingw64/mingw-w64-x86_64-SDL2_image\
 mingw64/mingw-w64-x86_64-SDL2_mixer\
 mingw64/mingw-w64-x86_64-SDL2_ttf

# additional development tools
pacman -S mingw-w64-x86_64-clang\
 mingw-w64-x86_64-clang-tools-extra\
 mingw64/mingw-w64-x86_64-include-what-you-use\
 msys/python

# this linker is much faster on Windows
pacman -S mingw64/mingw-w64-x86_64-lld
```

### Build SDL2 From Scratch

If you'd prefer to build SDL, this might be helpful to you.

```
cd /tmp
git clone https://github.com/libsdl-org/SDL
cd SDL
git checkout SDL2
mkdir build
cd build
../configure
make
sudo make install
```

### Clangd Setup

Use https://github.com/nickdiego/compiledb to generate the json file clangd needs to debug.

Install with

```
pip install compiledb
# OR
python3 -m pip install compiledb
```

Then run

```
./compile-commands.sh
```

This should allow vscode /w clangd or other programs to ingest the file for usage in development.

### Localization

Localization files are generated with a python script. These should be dropped into the src/assets folder automatically and wont automatically override previous strings.

```
python3 scripts/scan_locstr.py --languages la en
```

The localization doesn't work for the MiyooA30 because the json library that the sdl 2 wrapper uses is not supported by the version of gcc on the distro.

### MiyooA30

This repo contains a toolchain for building to the MiyooA30 handheld. This was based off the article that can be found here: https://www.hydrogen18.com/blog/compiling-for-the-miyoo-a30-sdl2.html which has an associated repo here: https://codeberg.org/hydrogen18/miyooa30. That repo is included in this repo as an example under the miyooa30-toolchain folder.

This is all assuming you're running Spruce on your MiyooA30. It should probably work without that, but it hasn't been tested under any other launchers.

Setup the toolchain by building the docker container. Make sure you have docker installed and can run docker commands.

```
# Build the toolchain docker container.  This should create a dist folder with executable and assets.
./setup-miyooa30-toolchain.sh

# Run the build command
# for msys2 you can run this helper script
cd scripts && ./miyooa30-build.sh


# for mac/linux run this command (this wont create a tarfile for you, you'll have to do that yourself)
docker run --rm -it --mount type=bind,source="$(pwd)/src",target=/workspace/src miyooa30-toolchain bash -c "cd /workspace/src && source ~/config.sh && make miyooa30"

# If you want to debug the container, you can get a shell with this (linux/mac command, use script for MSYS2)
docker run --rm -it --mount type=bind,source="$(pwd)/src",target=/workspace/src --entrypoint bash miyooa30-toolchain
```

To get this running on a MiyooA30, copy the contents of the src/dist folder onto the sdcard. This would be the binary and assets folder into a folder called "ScoundrelGame" at the top level (like "/mnt/sdcard/ScoundrelGame/SCOUNDREL" and "/mnt/sdcard/ScoundrelGame/assets"). Then copy the file "scripts/Scoundrel.sh" into "Roms/PORTS/Scoundrel.sh".

```
tar -xzvf VortexXeGame.tar.gz
```

NOTE: Don't put the ScoundrelGame folder in the Roms/PORTS directory, this will cause it to not work, even if you edit the sh file to point to it. For whatever reason, homebrew doesn't work from there (at least when using Spruce). This caused me much frustration.

You can add a Scoundrel.png image of the game in the Roms/PORTS/ image folder if you want an image for the game to appear when you hover it.

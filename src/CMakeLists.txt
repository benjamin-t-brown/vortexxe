cmake_minimum_required(VERSION 3.15)
project(VORTEXXE LANGUAGES CXX)

# Set C++ standard and basic flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the current directory (and add more if needed)
include_directories(${CMAKE_SOURCE_DIR})

# Option to select an Emscripten build
option(EMSCRIPTEN_BUILD "Build for Emscripten" OFF)

if(EMSCRIPTEN_BUILD)
  message(STATUS "Configuring for Emscripten")
  # These emscripten flags match your EMCC_LIBS_CLIENT and EMCC_EXPORTED settings.
  add_compile_options(-Oz -flto -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 
                      -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1 
                      -s SAFE_HEAP=0 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1
                      -s INITIAL_MEMORY=326565888 -s ENVIRONMENT=web -s JS_MATH=0)
  add_link_options(-s EXPORTED_FUNCTIONS='["_main","_enableSound","_disableSound","_setVolume","_setKeyDown","_setKeyUp","_setKeyStatus"]'
                   -s EXPORTED_RUNTIME_METHODS='["ccall"]')
endif()

# Set common compile flags
if(NOT EMSCRIPTEN_BUILD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall")
endif()

# Define source groups
set(MAIN_FILES main.cpp)

# Client library code and non-lib code
set(CLIENT_LIB_CODE LibHTML.cpp)
set(CLIENT_NONLIB_CODE
    game/GameManager.cpp
    client/Render.cpp
    client/Ui.cpp
    client/ProgressBar.cpp
)
set(CLIENT_CODE ${CLIENT_LIB_CODE} ${CLIENT_NONLIB_CODE})

# Shared library code
set(SHARED_LIB_CODE
    lib/sdl2wrapper/Animation.cpp
    lib/sdl2wrapper/AssetLoader.cpp
    lib/sdl2wrapper/Events.cpp
    lib/sdl2wrapper/SDL2Includes.cpp
    lib/sdl2wrapper/Sound.cpp
    lib/sdl2wrapper/Sprite.cpp
    lib/sdl2wrapper/Store.cpp
    lib/sdl2wrapper/Window.cpp
    lib/sdl2wrapper/Timer.cpp
    lib/sdl2wrapper/Gauge.cpp
    lib/sdl2wrapper/Transform.cpp
    lib/sdl2wrapper/Logger.cpp
    lib/sdl2wrapper/Localization.cpp
)
set(SHARED_CODE ${SHARED_LIB_CODE})

# Combine all source files
set(ALL_SOURCES ${MAIN_FILES} ${CLIENT_CODE} ${SHARED_CODE})

# Create executable target
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# OS-specific linking and include settings
if(EMSCRIPTEN_BUILD)
  # The emscripten build (using em++) should handle SDL2 linking from the flags above.
  # Assets preloading, etc. can be handled via additional commands or CMake scripting.
elseif(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE mingw32 SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer)
elseif(APPLE)
  target_include_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/include /usr/local/include)
  target_link_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/lib /usr/local/lib)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer)
endif()

# Optionally define custom targets mimicking your Makefile targets

# 'run' target
add_custom_target(run
  COMMAND $<TARGET_FILE:${PROJECT_NAME}>
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# 'clean' is handled automatically by CMake's build system.
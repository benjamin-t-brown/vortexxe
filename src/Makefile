EXE=VORTEXXE

MAIN_ALL=main.cpp

CLIENT_LIB_CODE=\
LibHTML.cpp

SHARED_LIB_CODE=\
lib/hiscore/hiscore.cpp\
lib/sdl2wrapper/Animation.cpp\
lib/sdl2wrapper/AssetLoader.cpp\
lib/sdl2wrapper/Events.cpp\
lib/sdl2wrapper/SDL2Includes.cpp\
lib/sdl2wrapper/Sprite.cpp\
lib/sdl2wrapper/Store.cpp\
lib/sdl2wrapper/Window.cpp\
lib/sdl2wrapper/Timer.cpp\
lib/sdl2wrapper/Gauge.cpp\
lib/sdl2wrapper/Transform.cpp\
lib/sdl2wrapper/Logger.cpp\
lib/sdl2wrapper/Localization.cpp

CLIENT_NONLIB_CODE=\
game/GameManager.cpp\
client/Render.cpp\
client/Ui.cpp\
client/ProgressBar.cpp\

CLIENT_CODE=\
$(CLIENT_LIB_CODE)\
$(CLIENT_NONLIB_CODE)

SHARED_CODE=\
$(SHARED_LIB_CODE)\
$(SHARED_NONLIB_CODE)

TEST_CODE=\

TEST_CODE_UI=\

MAIN_ALL_OBJECT=$(MAIN_ALL:.cpp=.o)

CLIENT_OBJECTS=$(CLIENT_CODE:.cpp=.o)
SERVER_OBJECTS=$(SERVER_CODE:.cpp=.o)
SHARED_OBJECTS=$(SHARED_CODE:.cpp=.o)
TEST_OBJECTS=$(TEST_CODE:.cpp=.o)
TEST_UI_OBJECTS=$(TEST_CODE_UI:.cpp=.o)

CLIENT_DEPENDS := $(patsubst %.cpp,%.d,$(CLIENT_CODE))
SERVER_DEPENDS := $(patsubst %.cpp,%.d,$(SERVER_CODE))
SHARED_DEPENDS := $(patsubst %.cpp,%.d,$(SHARED_CODE))
TEST_DEPENDS := $(patsubst %.cpp,%.d,$(TEST_CODE))
TEST_UI_DEPENDS := $(patsubst %.cpp,%.d,$(TEST_CODE_UI))

ALL_CLIENT_OBJECTS=$(CLIENT_OBJECTS) $(SHARED_OBJECTS)
ALL_SERVER_OBJECTS=$(SERVER_OBJECTS) $(SHARED_OBJECTS)

ALL_CLIENT_DEPENDS=$(CLIENT_DEPENDS) $(SHARED_DEPENDS)
ALL_SERVER_DEPENDS=$(SERVER_DEPENDS) $(SHARED_DEPENDS)

EMCC_LIBS_CLIENT=\
-Oz\
-flto\
-s USE_SDL=2\
-s USE_SDL_IMAGE=2\
-s USE_SDL_MIXER=2\
-s SDL2_IMAGE_FORMATS='["png"]'\
-s USE_SDL_TTF=2\
-s ALLOW_MEMORY_GROWTH=1\
-s SAFE_HEAP=0\
-s DEMANGLE_SUPPORT=1\
-s ASSERTIONS=1\
-s INITIAL_MEMORY=326565888\
-s ENVIRONMENT=web\
-s JS_MATH=0\
-DSDL2WRAPPER_ENABLE_LOCALIZATION

EMCC_EXPORTED=\
-s EXPORTED_FUNCTIONS='[\
 "_main",\
 "_enableSound",\
 "_disableSound",\
 "_setVolume",\
 "_setKeyDown",\
 "_setKeyUp",\
 "_setKeyStatus"\
]'\
-s EXPORTED_RUNTIME_METHODS=['ccall']

FLAGS=-O0 -g -Wall -std=c++17 -I. -DSDL2WRAPPER_ENABLE_LOCALIZATION
LINK_FLAGS=

ifeq ($(OS),Windows_NT)
	LIBS=-mconsole -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
# If you get an 'lld error', remove the -fuse-ld=lld flag from the LINK_FLAGS variable
# NOTE it will likely link much slower on Windows.
	LINK_FLAGS+=-fuse-ld=lld 
else
	UNAME_S=$(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
    LIBS=-L/opt/homebrew/lib -L/usr/local/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
		FLAGS+= -I/opt/homebrew/include -I/usr/local/include
	else
		LIBS=-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
    endif
endif

.PHONY: $(EXE) all
$(EXE): $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(SHARED_OBJECTS) $(MAIN_ALL_OBJECT)
	$(CXX) $(FLAGS) $(LINK_FLAGS) $(MAIN_ALL_OBJECT) $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(SHARED_OBJECTS) -o $(EXE) $(LIBS)

all: $(EXE)

-include $(ALL_CLIENT_DEPENDS)
-include $(ALL_SERVER_DEPENDS)

%.o: %.cpp
	$(CXX) $(FLAGS) -MMD -MP -c $< -o $@

run:
	./$(EXE)

temp:
	mkdir -p .build

js: temp
	rm -rf dist
	em++ -I. -std=c++17 $(MAIN_ALL) $(CLIENT_CODE) $(SERVER_CODE) $(SHARED_CODE) $(EMCC_LIBS_CLIENT) $(EMCC_EXPORTED) --preload-file assets -o .build/$(EXE).js
	mkdir -p dist
	mv -v .build/* ../web
	rm -fd .build

# This is intended to be used from within the miyooa30-toolchain docker container
miyooa30:
	rm -rf dist
	$(CXX) -DMIYOOA30 -O3 -I. -std=c++17 -Wall -Wno-psabi $$SDL_INCLUDE -o $(EXE) $$MARCH $$SDL_LIBS $$SDL_LIB_PATHS -Wl,-verbose $(MAIN_ALL) $(CLIENT_CODE) $(SERVER_CODE) $(SHARED_CODE)
	mkdir -p dist/assets
	cp -v $(EXE) dist
	cp -r assets/*.png dist/assets
	cp -r assets/*.ttf dist/assets
	cp -r assets/*.wav dist/assets
	cp -r assets/*.txt dist/assets

clean:
	rm -f $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(SHARED_OBJECTS) $(MAIN_ALL_OBJECT)
	rm -f $(CLIENT_DEPENDS) $(SERVER_DEPENDS) $(SHARED_DEPENDS)
	rm -f precompiled.h.gch
	rm -f $(EXE) $(EXE).exe
	rm -fd .build
	rm -rf dist
	rm -rf main.d

clean_nonlib:
	rm -f $(CLIENT_NONLIB_CODE:.cpp=.o) $(SERVER_NOLIB_CODE:.cpp=.o) $(SHARED_NONLIB_CODE:.cpp=.o) $(MAIN_ALL_OBJECT)
	rm -f $(CLIENT_NONLIB_CODE:.cpp=.d) $(SERVER_NOLIB_CODE:.cpp=.d) $(SHARED_NONLIB_CODE:.cpp=.d)
	rm -f precompiled.h.gch
	rm -f $(EXE) $(EXE).exe
	rm -fd .build
	rm -rf dist
	rm -rf main.d

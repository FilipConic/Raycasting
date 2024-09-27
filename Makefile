GPP = g++ -std=c++2a
FLAGS = -Wall -Wextra
INCLUDE_PATH = -I src\include
LIB_PATH = -L src\lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

main.exe: main.o vec2.o color.o mouse.o keyboard.o scene.o player.o image_loader.o
	${GPP} ${FLAGS} $^ -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

main.o: main.cpp vec2.o color.o keyboard.o scene.o player.o image_loader.o
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

player.o: Player.cpp scene.o image_loader.o
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

image_loader.o: SDL_Components\ImageLoader.cpp scene.o
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

scene.o: SDL_Components\Scene.cpp color.o vec2.o
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

keyboard.o: SDL_Components\Keyboard.cpp
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

mouse.o: SDL_Components\Mouse.cpp vec2.o
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

color.o: SDL_Components\Color.cpp
	${GPP} ${FLAGS} -c $< -o $@

vec2.o: SDL_Components\Vec2.cpp
	${GPP} ${FLAGS} -c $< -o $@

clean:
	del *.o
	del *.exe
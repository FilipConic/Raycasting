GPP = g++ -std=c++2a
FLAGS = -Wall -Wextra
INCLUDE_PATH = -I src\include
LIB_PATH = -L src\lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

BUILD_FOLDER = .\build

main.exe: ${BUILD_FOLDER}\main.o ${BUILD_FOLDER}\vec2.o ${BUILD_FOLDER}\color.o ${BUILD_FOLDER}\mouse.o ${BUILD_FOLDER}\keyboard.o ${BUILD_FOLDER}\scene.o ${BUILD_FOLDER}\player.o ${BUILD_FOLDER}\image_loader.o
	${GPP} ${FLAGS} $^ -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\main.o: main.cpp ${BUILD_FOLDER}\vec2.o ${BUILD_FOLDER}\color.o ${BUILD_FOLDER}\keyboard.o ${BUILD_FOLDER}\scene.o ${BUILD_FOLDER}\player.o ${BUILD_FOLDER}\image_loader.o ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\player.o: Player.cpp ${BUILD_FOLDER}\scene.o ${BUILD_FOLDER}\image_loader.o ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\image_loader.o: SDL_Components\ImageLoader.cpp ${BUILD_FOLDER}\scene.o ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\scene.o: SDL_Components\Scene.cpp ${BUILD_FOLDER}\color.o ${BUILD_FOLDER}\vec2.o ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\keyboard.o: SDL_Components\Keyboard.cpp ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\mouse.o: SDL_Components\Mouse.cpp ${BUILD_FOLDER}\vec2.o ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@ ${INCLUDE_PATH} ${LIB_PATH} ${LIBS}

${BUILD_FOLDER}\color.o: SDL_Components\Color.cpp ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@

${BUILD_FOLDER}\vec2.o: SDL_Components\Vec2.cpp ${BUILD_FOLDER}
	${GPP} ${FLAGS} -c $< -o $@

${BUILD_FOLDER}:
	mkdir $@

clean:
	del ${BUILD_FOLDER}\*.o
	del *.exe
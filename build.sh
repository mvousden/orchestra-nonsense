#!/bin/sh
set -e
clang -Wall -Werror -pedantic -g -std=c89 -I/usr/include/SDL2/ music.c -c -o music.o
clang -Wall -Werror -pedantic -g -std=c89 -I/usr/include/SDL2/ serial_solo.c -c -o serial_solo.o
clang -Wall -Werror -pedantic -g -std=c89 -I/usr/include/SDL2/ -lSDL2 -lm music.o serial_solo.o -o serial_solo
clang -Wall -Werror -pedantic -g -std=c89 -I/usr/include/SDL2/ orchestra.c -c -o orchestra.o
clang -Wall -Werror -pedantic -g -std=c89 -I/usr/include/SDL2/ -lSDL2 -lm music.o orchestra.o -o orchestra

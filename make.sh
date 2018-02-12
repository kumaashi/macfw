rm ./a.out
g++ -Wall -std=c++14 main.cpp -g -O2 -lSDLmain -lSDL -Wl,-framework,Cocoa -Wl,-framework,OpenGL
./a.out


cl main.cpp /EHsc /Ox /GS- /Iinclude  /c /MD
link main.obj /SUBSYSTEM:CONSOLE opengl32.lib lib\SDL.lib lib\SDLmain.lib legacy_stdio_definitions.lib

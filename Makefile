FILENAMES = main.cpp screen.cpp

CC = g++.exe

INCLUDE_PATHS = -IC:\MinGW\include

LIBRARY_PATHS = -LC:\MinGW\lib

COMPILER_FLAGS = -Wall

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

OBJNAME = game

all:
	$(CC) $(FILENAMES) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) && ./a.exe

# g++.exe $fileName -w -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer && ./a.exe
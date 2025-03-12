# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Libraries for different platforms
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    LIBS = -lglfw -lGLEW -lGL -lm
endif
ifeq ($(UNAME_S), Darwin)  # macOS
    LIBS = -lglfw -lGLEW -framework OpenGL -lm
endif
ifeq ($(OS), Windows_NT)  # Windows (MinGW)
    LIBS = -lglfw3 -lglew32 -lopengl32 -lm
endif

# Output and source files
TARGET = space2d 
SRC = main.c

# Build rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)


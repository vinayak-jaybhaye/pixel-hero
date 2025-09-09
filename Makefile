# Makefile for Enhanced Super Mario Platformer

CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11
LDFLAGS = -lGL -lGLU -lglut -lm

# Source files
SOURCES = main.cpp game.cpp player.cpp platform.cpp collectible.cpp \
          particle.cpp graphics.cpp renderer.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
TARGET = pixel_hero

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all clean run rebuild
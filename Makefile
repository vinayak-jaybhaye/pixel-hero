# Makefile

CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11 -I include
LDFLAGS = -lGL -lGLU -lglut -lm

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Source files
SOURCES = main.cpp game.cpp player.cpp platform.cpp collectible.cpp \
          particle.cpp graphics.cpp renderer.cpp

# Object files (placed inside build/)
OBJECTS = $(addprefix $(BUILDDIR)/, $(SOURCES:.cpp=.o))

# Executable name
TARGET = $(BUILDDIR)/pixel_hero

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files from src/ into build/
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if missing
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean build files
clean:
	rm -rf $(BUILDDIR)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all clean run rebuild
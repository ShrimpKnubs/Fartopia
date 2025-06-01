# Makefile for Emergent Kingdoms with SFML - Modular Systems Structure + Medieval Vegetation

# Compiler and flags
CXX = g++
# ADDED -fopenmp for OpenMP support
CXXFLAGS = -std=c++17 -Wall -Wextra -g -fopenmp
# Include paths for SFML and project src
# Adjust SFML_INCLUDE_DIR if SFML is installed elsewhere
# For WSL/Linux, pkg-config is preferred
SFML_CFLAGS = $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system)
SFML_LIBS = $(shell pkg-config --libs sfml-graphics sfml-window sfml-system)

# If pkg-config is not available or SFML is in a custom location:
# SFML_DIR = /path/to/your/sfml/installation
# SFML_CFLAGS = -I$(SFML_DIR)/include
# SFML_LIBS = -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system

CPPFLAGS = -Isrc $(SFML_CFLAGS)
# ADDED -fopenmp for OpenMP support
LDFLAGS = $(SFML_LIBS) -fopenmp

# Project name
TARGET = emergent_kingdoms

# Source files organized by modular structure
SRCS = \
    src/main.cpp \
    src/Core/Game.cpp \
    src/Core/Renderer.cpp \
    src/World/Map.cpp \
    src/World/Tile.cpp \
    src/World/TileAssigner.cpp \
    src/Entities/Entity.cpp \
    src/Entities/Fartling.cpp \
    src/World/GenerationSteps/BaseHeightGenerator.cpp \
    src/World/GenerationSteps/BorderWallPlacer.cpp \
    src/World/GenerationSteps/HydraulicEroder.cpp \
    src/World/GenerationSteps/SlopeAspectCalculator.cpp \
    src/World/GenerationSteps/ThermalEroder.cpp \
    src/World/Systems/Land/LandTileAssigner.cpp \
    src/World/Systems/Mountains/MountainGenerator.cpp \
    src/World/Systems/Mountains/MountainTileAssigner.cpp \
    src/World/Systems/Rivers/RiverNetworkSimulator.cpp \
    src/World/Systems/Rivers/RiverTileAssigner.cpp \
    src/World/Systems/Lakes/LakeFormer.cpp \
    src/World/Systems/Lakes/LakeTileAssigner.cpp \
    src/World/Systems/Vegetation/VegetationGenerator.cpp \
    src/World/Systems/Vegetation/VegetationTileAssigner.cpp \
    src/World/Systems/Vegetation/MultiTileObjects/BaseVegetationObject.cpp \
    src/World/Systems/Vegetation/MultiTileObjects/VegetationObjectManager.cpp \
    src/World/Systems/Vegetation/MultiTileObjects/Trees/AncientOakTree.cpp \
    src/World/Systems/Vegetation/MultiTileObjects/Trees/YoungTree.cpp \
    src/World/Systems/Vegetation/MultiTileObjects/Boulders/ResourceBoulder.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
# Note: CXXFLAGS already contains -fopenmp, so it's applied during compilation too

# Rule for src/*.cpp
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/Core/*.cpp
src/Core/%.o: src/Core/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/*.cpp
src/World/%.o: src/World/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/Entities/*.cpp
src/Entities/%.o: src/Entities/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/GenerationSteps/*.cpp
src/World/GenerationSteps/%.o: src/World/GenerationSteps/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rules for modular Systems
# Rule for src/World/Systems/Land/*.cpp
src/World/Systems/Land/%.o: src/World/Systems/Land/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Mountains/*.cpp
src/World/Systems/Mountains/%.o: src/World/Systems/Mountains/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Rivers/*.cpp
src/World/Systems/Rivers/%.o: src/World/Systems/Rivers/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Lakes/*.cpp
src/World/Systems/Lakes/%.o: src/World/Systems/Lakes/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Vegetation/*.cpp - NEW!
src/World/Systems/Vegetation/%.o: src/World/Systems/Vegetation/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Vegetation/MultiTileObjects/*.cpp - NEW!
src/World/Systems/Vegetation/MultiTileObjects/%.o: src/World/Systems/Vegetation/MultiTileObjects/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Vegetation/MultiTileObjects/Trees/*.cpp - NEW!
src/World/Systems/Vegetation/MultiTileObjects/Trees/%.o: src/World/Systems/Vegetation/MultiTileObjects/Trees/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule for src/World/Systems/Vegetation/MultiTileObjects/Boulders/*.cpp - NEW!
src/World/Systems/Vegetation/MultiTileObjects/Boulders/%.o: src/World/Systems/Vegetation/MultiTileObjects/Boulders/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
	# Clean up .o files in all directories
	find src -name "*.o" -type f -delete

# Phony targets
.PHONY: all clean
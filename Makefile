APP_NAME = supertux
CXX = c++

# SDL2 flags using pkg-config
CXXFLAGS = -std=c++11 -Wall -O0 -g $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer)
LDFLAGS  = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer) -lz -framework OpenGL

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRCS:.cpp=.o)

all: $(APP_NAME)

$(APP_NAME): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(SRC_DIR)/*.o $(APP_NAME)
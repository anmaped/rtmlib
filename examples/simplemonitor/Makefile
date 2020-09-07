SRC_DIR := .
OBJ_DIR := .
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -std=c++11 -pthread -DDEBUG=1
CPPFLAGS := -std=c++11 -I../../src/ -DDEBUG=1
CXXFLAGS := 

simplemonitor: $(OBJ_FILES) ../../src/periodicmonitor.cpp
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f simplemonitor $(OBJ_FILES)

SRC_DIR := .
OBJ_DIR := .
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -pthread -latomic
CPPFLAGS := -std=c++11 -mcx16 -I../../src/ -DDEBUG=1 # -mcx16 flag is required for gcc versions <7
CXXFLAGS := 

simplemonitor: $(OBJ_FILES)
	g++ $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f simplemonitor $(OBJ_FILES)

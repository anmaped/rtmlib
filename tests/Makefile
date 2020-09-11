SRC_DIR := .
OBJ_DIR := .
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -latomic -mcx16 # -mcx16 flag is required for gcc versions <7
CPPFLAGS := -std=c++11 -mcx16 -I../src/ #-DDEBUG=3
CXXFLAGS := 

SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp' | sed -r "s/.+\/(.+)\..+/\1/")
MAIN_FILE := $(patsubst %, extern int %();, $(SOURCES))
MAIN_FILE+=int main() {
MAIN_FILE+=$(patsubst %, %();, $(SOURCES))
MAIN_FILE+=return 0; }

Y := $(shell [ ! -f rtmlib_unittests.c ] && echo "$(MAIN_FILE)" > rtmlib_unittests.c)

rtmlib_unittests: $(OBJ_FILES) $(SRC_DIR)/rtmlib_unittests.c
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f rtmlib_unittests $(OBJ_FILES) rtmlib_unittests.c

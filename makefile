CXX = g++
CXXFLAGS = -O3 -w -I./include

CORE_SRC = ./src/repair.cpp ./src/main.cpp

BIN_DIR = build
TEST_OUT = $(BIN_DIR)/repair

.PHONY: all clean


all: $(BIN_DIR) $(TEST_OUT)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_OUT): $(TEST_SRC) $(CORE_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(BIN_DIR)
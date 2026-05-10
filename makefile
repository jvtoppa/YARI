CXX = g++
CXXFLAGS = -O3 -w -I./include

REPAIR_SRC = ./src/repair.cpp ./src/main.cpp
DESPAIR_SRC = ./src/despair.cpp
CHECK_SRC = ./src/check_correctness.cpp

BIN_DIR = build

REPAIR_OUT = $(BIN_DIR)/repair
DESPAIR_OUT = $(BIN_DIR)/despair
CHECK_OUT = $(BIN_DIR)/check_correctness

.PHONY: all clean

all: $(BIN_DIR) $(REPAIR_OUT) $(DESPAIR_OUT) $(CHECK_OUT)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(REPAIR_OUT): $(REPAIR_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(DESPAIR_OUT): $(DESPAIR_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# New rule for check_correctness
$(CHECK_OUT): $(CHECK_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(BIN_DIR)
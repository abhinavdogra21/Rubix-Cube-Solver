# Custom Kociemba Two-Phase Algorithm Solver
# Optimized for high-performance cube solving with cross-platform support

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra -DNDEBUG

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS specific flags
    CXXFLAGS += -mmacosx-version-min=10.9
    LDFLAGS = -lpthread
else ifeq ($(UNAME_S),Linux)
    # Linux specific flags
    LDFLAGS = -lpthread
else
    # Windows/MinGW
    LDFLAGS = -lpthread -static
endif

# Source and object files
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = kociemba-solver

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	mkdir -p ../bin
	cp $(TARGET) ../bin/

# Development targets
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

test: $(TARGET)
	./$(TARGET) solve DRLUUBFBRBLURRLRUBLRDDFDLFUFUFFDBRDUBRUFLLFDDBFLUBLRBD

.PHONY: all clean install debug test


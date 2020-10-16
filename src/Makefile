CXX=clang++
CXXFLAGS=-g -std=c++1z
BIN=prog

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^
ifndef TEST_CLANG
	@echo "Please install clang++"
endif

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)
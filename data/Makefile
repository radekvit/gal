CXX=g++
APPNAME=gen
INCLUDE=../include
SRC=../src
CXXFLAGS= -std=c++17 -Wall -Wextra -pedantic -I. -I $(INCLUDE) -I $(SRC) -O3 -DNDEBUG

N=100
E=300

all: $(APPNAME)

$(APPNAME): gen.cpp $(SRC)/random.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm $(APPNAME)

generate: $(APPNAME)
	./$(APPNAME) $(N) $(E) g-$(N)-$(E).txt

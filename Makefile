CXX=g++
APPNAME=gal2018
INCLUDE=include
LIBDIR = 
LIBINCLUDE = $(LIBDIR)/include
LIBSRC = $(LIBDIR)/src
SRC=src
CXXFLAGS += -std=c++17 -Wall -Wextra -pedantic -I. -I $(INCLUDE) -I $(SRC)
OBJ=obj
DOC = docs
$(shell mkdir -p $(OBJ))

HEADERS=$(wildcard $(SRC)/*.hpp) $(wildcard $(INCLUDE)/*.h)
LIBHEADERS=$(wildcard $(LIBSRC)/*.hpp)
OBJFILES=$(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(wildcard $(SRC)/*.cpp))

.PHONY: all format clean debug build test pack doc run libbuild cleanall

all: deploy

init:
	git submodule init
	git submodule update

build: $(APPNAME)

debug: CXXFLAGS+=-g -O0
debug: build

deploy: CXXFLAGS+=-O3 -DNDEBUG
deploy: build

$(APPNAME): $(OBJFILES)
	$(CXX) $(CXXFLAGS) $(OBJFILES) -o $@ $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.cpp $(HEADERS) $(LIBHEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -r $(OBJFILES) $(APPNAME)
	$(MAKE) -C data clean

format:
	-clang-format -style=file -i $(SRC)/*.cpp $(SRC)/*.hpp $(INCLUDE)/*.h

test:
	make -C test test

doc:
	make -C $(DOC)

clean:

run: all
	./$(APPNAME)

# Project: sd_util
# Makefile created by Dev-C++ 5.11

CPP      = g++ -g
CC       = gcc -g
OBJ      = bin/serialize.o 
LINKOBJ  = bin/serialize.o
LIBS     = 
INCS     =
CXXINCS  = 
BIN      = lib/libsdutil.a
INCLUDE = -I./
CXXFLAGS = $(CXXINCS) -std=gnu++1y $(INCLUDE)
CFLAGS   = $(INCS) -std=gnu++1y
RM       = rm -f


.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(LINKOBJ)
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)


bin/serialize.o: src/serialize.cpp include/external/serialize.hpp
	$(CPP) -c src/serialize.cpp -o bin/serialize.o $(CXXFLAGS)




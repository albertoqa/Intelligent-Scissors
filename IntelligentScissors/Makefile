CXX = g++
OBJ = obj
INC = include
BIN = bin
LIB = lib
SRC = src
CXXFLAGS =-Wall -g -c -O3
OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

Segmentation:$(OBJ)/MainProgram.o $(OBJ)/IntelligentScissors.o $(OBJ)/Window.o
	$(CXX) -o Segmentation $^ $(LIBS)

$(OBJ)/MainProgram.o: $(SRC)/MainProgram.cpp $(INC)/IntelligentScissors.h $(INC)/Window.h
	$(CXX) $(CXXFLAGS) $(SRC)/MainProgram.cpp -o $(OBJ)/MainProgram.o -I$(INC)

$(OBJ)/IntelligentScissors.o: $(SRC)/IntelligentScissors.cpp $(INC)/IntelligentScissors.h
	$(CXX) $(CXXFLAGS) $(SRC)/IntelligentScissors.cpp -o $(OBJ)/IntelligentScissors.o -I$(INC)

$(OBJ)/Window.o: $(SRC)/Window.cpp $(INC)/Window.h
	$(CXX) $(CXXFLAGS) $(SRC)/Window.cpp -o $(OBJ)/Window.o -I$(INC)

clean:
	 rm -f *~; rm -f $(SRC)/*~; rm -f $(INC)/*~; rm -f $(OBJ)/*; rm -f Segmentation

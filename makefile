CXX=g++
CXXFLAGS=-g -ansi -pedantic -Wall -Werror -std=c++14

OBJECTS=main.o rv32i_decode.o memory.o hex.o registerfile.o rv32i_hart.o cpu_single_hart.o

rv32i: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o rv32i $(OBJECTS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

rv32i_decode.o: rv32i_decode.cpp
	$(CXX) $(CXXFLAGS) -c -o rv32i_decode.o rv32i_decode.cpp

memory.o: memory.cpp
	$(CXX) $(CXXFLAGS) -c -o memory.o memory.cpp

hex.o: hex.cpp
	$(CXX) $(CXXFLAGS) -c -o hex.o hex.cpp

registerfile.o: registerfile.cpp
	$(CXX) $(CXXFLAGS) -c -o registerfile.o registerfile.cpp

rv32i_hart.o: rv32i_hart.cpp
	$(CXX) $(CXXFLAGS) -c -o rv32i_hart.o rv32i_hart.cpp

cpu_single_hart.o: cpu_single_hart.cpp
	$(CXX) $(CXXFLAGS) -c -o cpu_single_hart.o cpu_single_hart.cpp

clean:
	rm -f rv32i $(OBJECTS)
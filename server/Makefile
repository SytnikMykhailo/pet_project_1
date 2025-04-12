CXX = g++
CXXFLAGS = -std=c++20 -Wall -W -Werror
LDFLAGS = -pthread

program: color.o myNoiseGeneratorServer.o
	$(CXX) $^ -o $@ $(LDFLAGS)

color.o: src/source/color.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

myNoiseGeneratorServer.o: src/myNoiseGeneratorServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe 2>nul
CXX = g++
CXXFLAGS = -std=c++20 -Wall -W -Werror
LDFLAGS = -pthread

program: image.o myNoiseGeneratorServer.o
	$(CXX) $^ -o $@ $(LDFLAGS)

image.o: src/source/image.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

myNoiseGeneratorServer.o: src/myNoiseGeneratorServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe 2>nul
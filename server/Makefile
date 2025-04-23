CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -W
LDFLAGS = -pthread -mconsole


program: myNoiseGeneratorServer.o
	$(CXX) $^ -o $@ $(LDFLAGS)

myNoiseGeneratorServer.o: src/myNoiseGeneratorServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	del /Q *.o program.exe 2>null || exit 0
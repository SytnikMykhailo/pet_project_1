CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -W
LDFLAGS = -pthread -mconsole -lws2_32

program: myNoiseGeneratorClient.o client.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(COPY_DLL)

myNoiseGeneratorClient.o: src/myNoiseGeneratorClient.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

client.o: src/source/client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe 2>nul || exit 0
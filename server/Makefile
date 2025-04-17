CXX = g++
CXXFLAGS = -std=c++20 -Wall -W -Werror
LDFLAGS = -pthread

program: color.o myNoiseGeneratorServer.o imageloader.o BMPImage.o JPEGImage.o PNGImage.o
	$(CXX) $^ -o $@ $(LDFLAGS)

color.o: src/source/color.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

myNoiseGeneratorServer.o: src/myNoiseGeneratorServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

BMPImage.o: src/source/BMPImage.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

PNGImage.o: src/source/PNGImage.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

JPEGImage.o: src/source/JPEGImage.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

imageloader.o: src/source/imageloader.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe 2>nul
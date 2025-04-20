CXX = g++
CXXFLAGS = -std=c++20 -Wall -W -Werror -Iexternal_libraries/zlib/include
LDFLAGS = -pthread -mconsole -L./external_libraries/zlib/lib -lzlib

ifeq ($(OS),Windows_NT)
    COPY_DLL = copy external_libraries\\zlib\\bin\\libzlib.dll .\\ >nul
else
    COPY_DLL = cp external_libraries/zlib/bin/libzlib.dll ./
endif

program: color.o myNoiseGeneratorImageHandler.o imageloader.o BMPImage.o JPEGImage.o PNGImage.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(COPY_DLL)

color.o: src/source/color.cpp src/headers/color.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

myNoiseGeneratorImageHandler.o: src/myNoiseGeneratorImageHandler.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

BMPImage.o: src/source/BMPImage.cpp src/headers/BMPImage.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

PNGImage.o: src/source/PNGImage.cpp src/headers/PNGImage.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

JPEGImage.o: src/source/JPEGImage.cpp src/headers/JPEGImage.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

imageloader.o: src/source/imageloader.cpp src/headers/imageloader.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe libzlib.dll 2>nul || exit 0
	del /Q build\* 2>nul || exit 0
	rmdir /S /Q build 2>nul || exit 0

dependencies:
	@if not exist "build" mkdir build
	cd build && cmake -DZLIB_INSTALL_DIR=../external_libraries/zlib .. && cmake --build . --target install

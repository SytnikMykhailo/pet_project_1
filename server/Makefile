CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -W
LDFLAGS = -pthread -mconsole


ifeq ($(OS),Windows_NT)
    COPY_DLL = copy external_libraries\\sqlite\\bin\\sqlite3.dll .\\ >nul
else
    COPY_DLL = cp external_libraries/sqlite/bin/sqlite3.dll ./
endif

program: myNoiseGeneratorServer.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(COPY_DLL)

myNoiseGeneratorServer.o: src/myNoiseGeneratorServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	del /Q *.o program.exe sqlite3.dll 2>nul || exit 0
	del /Q build\* 2>nul || exit 0
	rmdir /S /Q build 2>nul || exit 0

sqlite_dependencies:
	@if not exist "build_sqlite" mkdir build_sqlite
	cd build_sqlite && cmake -DCMAKE_INSTALL_PREFIX=../external_libraries/sqlite .. && cmake --build . --target install

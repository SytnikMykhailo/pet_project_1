CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -W -Iexternal_libraries
LDFLAGS = -pthread -mconsole -Lexternal_libraries/lib -lsqlite3


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
	del /Q build_sqlite\* 2>nul || exit 0
	rmdir /S /Q build_sqlite 2>nul || exit 0

dependencies:
	@if not exist "build_sqlite" mkdir build_sqlite
	cd build_sqlite && cmake -DCMAKE_INSTALL_PREFIX=../external_libraries/sqlite .. && cmake --build .

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -W \
           -Isrc/external_libraries/lib \
           -Isrc/headers
LDFLAGS = -pthread -mconsole -Lsrc/external_libraries/lib -lsqlite3 -lws2_32


#ifeq ($(OS),Windows_NT)
#    COPY_DLL = copy external_libraries\\sqlite\\bin\\sqlite3.dll .\\ >nul
#else
#    COPY_DLL = cp external_libraries/sqlite/bin/sqlite3.dll ./
#endif

program: myNoiseGeneratorServer.o threaded_server.o server.o database.o hash_table.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(COPY_DLL)

myNoiseGeneratorServer.o: src/myNoiseGeneratorServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

threaded_server.o: src/source/threaded_server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

server.o: src/source/server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

database.o: src/source/database.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

hash_table.o: src/source/hash_table.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe sqlite3.dll 2>nul || exit 0
	del /Q build_sqlite\* 2>nul || exit 0
	rmdir /S /Q build_sqlite 2>nul || exit 0

dependencies:
	@if not exist "build_sqlite" mkdir build_sqlite
	cd build_sqlite && cmake -DCMAKE_INSTALL_PREFIX=../external_libraries/sqlite .. && cmake --build .

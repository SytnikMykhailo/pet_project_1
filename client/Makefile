CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -W -I../image_ops/external_libraries/zlib/include
LDFLAGS = -pthread -mconsole -lws2_32 -L../image_ops/external_libraries/zlib/lib -lzlib

ifeq ($(OS),Windows_NT)
    COPY_DLL = copy ..\\image_ops\\external_libraries\\zlib\\bin\\libzlib.dll .\\ >nul
else
    COPY_DLL = cp ../image_ops/external_libraries/zlib/bin/libzlib.dll ./
endif

program: myNoiseGeneratorClient.o client.o ../image_ops/color.o ../image_ops/JPEGImage.o ../image_ops/PNGImage.o ../image_ops/BMPImage.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(COPY_DLL)

myNoiseGeneratorClient.o: src/myNoiseGeneratorClient.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

client.o: src/source/client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o program.exe 2>nul || exit 0
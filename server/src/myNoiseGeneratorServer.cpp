#include <iostream>

int main(int argc, char** argv){
    if(argc == 4) return 1;

    std::cout << "Hello world! " << argv[0] << std::endl;
    return 0;
}
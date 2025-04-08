#include <iostream>


int main(int argc, char **argv){

    std::cout << "Hello, World!" << std::endl;
    std::cout << argv[argc - 1] << std::endl;
    
    return 0;
}
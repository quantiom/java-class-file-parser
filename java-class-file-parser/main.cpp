#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "parser/JavaClass.h"

int main() {
    const std::string file_path = "C:\\Test.class";

    std::ifstream instream(file_path, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
   
    auto java_class = JavaClass(data);
    java_class.parse();
}
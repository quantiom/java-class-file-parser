#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "parser/header/types/JavaClass.h"

int main() {
    const std::string file_path = "C:\\Test.class";

    std::ifstream instream(file_path, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
   
    std::unique_ptr<JavaClass> java_class(new JavaClass(data));
    java_class->parse(); // main stuff is done here

    for (const auto& field : java_class->get_fields()) {
        if (field->get_name() == "someField") {
            field->set_deprecated(false);
            field->remove_annotation("Ljdk/nashorn/internal/runtime/logging/Logger;", true);
        }
    }

    for (const auto& method : java_class->get_methods()) {
        if (method->get_name() == "main") {
            if (auto code_attribute = method->get_attribute<CodeAttribute>()) {
                const auto vec = std::get<CodeAttribute>(**code_attribute).get_code_string();
            
                for (const auto& line : vec) {
                    std::cout << line << std::endl;
                }
            }
        }
    }

    const auto new_bytes = java_class->get_bytes();

    std::ofstream("C:\\Users\\user\\Desktop\\NewTest.class", std::ios::binary).write((char*)new_bytes.data(), new_bytes.size());
}
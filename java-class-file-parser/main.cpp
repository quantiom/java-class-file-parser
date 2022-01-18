#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "parser/header/types/JavaClass.h"

int main() {
    const std::string file_path = "C:\\Test.class";

    std::ifstream instream(file_path, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
   
    std::unique_ptr<JavaClass> java_class = std::make_unique<JavaClass>(data);
    java_class->parse();

    for (const auto& field : java_class->get_fields()) {
        if (field->get_name() == "someStr") {
            const auto annotations = field->get_annotations(true);
            const auto test_annotation = std::find_if(annotations.begin(), annotations.end(), [](auto& annotation) {
                return annotation->get_name() == "Lme/quantiom/testing/TestAnnotation;";
            });

            if (test_annotation != annotations.end()) {
                std::cout << "Test Annotation type: " << java_class->get_constant_pool()->get_string((*test_annotation)->get_type_index()) << "\n";

                for (const auto& [e, v] : (*test_annotation)->get_element_value_pairs()) {
                    if (v->is_const_value()) {
                        std::cout << java_class->get_constant_pool()->get_string(e) << " : " << java_class->get_constant_pool()->get_string(v->m_const_value_index) << "\n";
                    }
                }

                field->remove_annotation(*test_annotation, true);
            }

            if (field->is_deprecated()) field->set_deprecated(false);
        }
    }

    for (const auto& method : java_class->get_methods()) {
        if (method->get_name() == "main") {
            if (auto code_attribute = method->get_attribute<CodeAttribute>()) {
                const auto bytecode = std::get<CodeAttribute>(**code_attribute).get_code_string();

                for (const auto& line : bytecode) {
                    std::cout << line << std::endl;
                }
            }
        }
    }

    const auto new_bytes = java_class->get_bytes();

    std::ofstream("C:\\Users\\user\\Desktop\\NewTest.class", std::ios::binary).write((char*)new_bytes.data(), new_bytes.size());
}
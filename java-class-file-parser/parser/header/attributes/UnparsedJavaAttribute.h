#pragma once
#include "JavaAttribute.h"

class UnparsedJavaAttribute : public JavaAttribute {
public:
    UnparsedJavaAttribute() : JavaAttribute() {};
    UnparsedJavaAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) : JavaAttribute(java_class, attribute) {};
    UnparsedJavaAttribute(JavaClass* java_class, u2 name_index) : JavaAttribute(java_class, name_index) {};

    void parse(std::unique_ptr<ByteReader>& reader) {
        this->m_info = reader->get_bytes();
    }

    std::vector<u1> get_bytes() {
        return this->m_info;
    }

private:
    std::vector<u1> m_info;
};
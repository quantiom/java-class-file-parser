#pragma once
#include "JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.15

struct DeprecatedAttribute : public JavaAttribute {
    // used in the variant
    DeprecatedAttribute() : JavaAttribute() {};

    DeprecatedAttribute(JavaClass* java_class, std::shared_ptr<JavaAttribute> attribute) : JavaAttribute(java_class, attribute) {};

    DeprecatedAttribute(JavaClass* java_class, u2 name_index, std::vector<u1> info)
        : JavaAttribute(java_class, name_index, info) {};
};
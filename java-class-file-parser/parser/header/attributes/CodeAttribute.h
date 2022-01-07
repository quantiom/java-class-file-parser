#pragma once
#include "JavaAttribute.h"

// https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3

struct ExceptionTableEntry {
    u2 m_start_pc;
    u2 m_end_pc;
    u2 m_handler_pc;
    u2 m_catch_type;
};

struct CodeAttribute : public JavaAttribute {
	CodeAttribute(JavaClass* java_class, JavaAttribute* attribute) : JavaAttribute(java_class, attribute) {
        this->parse();
	};

	CodeAttribute* parse();
	std::vector<u1> get_bytes();

	u2 m_max_stack;
	u2 m_max_locals;

    std::vector<u1> m_code;

    std::vector<ExceptionTableEntry> m_exception_table;
    
    std::vector<JavaAttribute*> m_attributes;
};
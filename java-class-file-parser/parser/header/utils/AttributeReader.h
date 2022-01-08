#pragma once
#include "../attributes/ParsedAttribute.h"

#include "ByteReader.h"
#include "FnvHash.h"

class AttributeReader : public ByteReader {
public:
	AttributeReader(JavaClass* java_class, std::vector<u1> bytes) : ByteReader(java_class, bytes) {};

	std::shared_ptr<ParsedAttribute> read_attribute();
};
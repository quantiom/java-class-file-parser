#pragma once
#include <iostream>
#include <vector>

#include "../defines.h"
#include "../attributes/JavaAttribute.h"

// simple wrapper used to write parsed class files back to bytes

struct ByteWriter {
	// methods for writing bytes to the vector
	// (no clue if there's a better way to do this)

	void write_u1(u1 byte) {
		this->m_bytes.push_back(byte);
	}

	void write_u2(u2 bytes) {
		this->m_bytes.push_back((u1)((bytes >> 8) & 255));
		this->m_bytes.push_back((u1)(bytes & 255));
	}

	void write_u4(u4 bytes) {
		this->m_bytes.push_back((u1)((bytes >> 24) & 255));
		this->m_bytes.push_back((u1)((bytes >> 16) & 255));
		this->m_bytes.push_back((u1)((bytes >> 8) & 255));
		this->m_bytes.push_back((u1)(bytes & 255));
	}

	void write_attributes(std::vector<JavaAttribute*> attributes) {
		this->write_u2(attributes.size());
		
		for (const auto& attribute : attributes) {
			this->write_u2(attribute->m_name_index);
			this->write_u4(attribute->m_info.size());

			for (const auto& byte : attribute->m_info) {
				this->write_u1(byte);
			}
		}
	}

	// bytes
	std::vector<u1> m_bytes;
};
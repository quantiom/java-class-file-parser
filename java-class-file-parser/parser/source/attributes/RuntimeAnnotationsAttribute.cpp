#include "../../header/attributes/RuntimeAnnotationsAttribute.h"
#include "../../header/types/JavaClass.h"
#include "../../header/types/JavaAnnotation.h"
#include "../../header/utils/ByteWriter.h"

void RuntimeAnnotationsAttribute::parse() {
	this->m_is_visible = this->get_name() == "RuntimeVisibleAnnotations";

	const auto num_annotations = this->read_u2();

	for (auto i = 0; i < num_annotations; i++) {
		this->m_annotations.push_back(this->parse_annotation());
	}
}

std::vector<u1> RuntimeAnnotationsAttribute::get_bytes() {
	std::unique_ptr<ByteWriter> writer(new ByteWriter());

	writer->write_u2((u2)this->m_annotations.size());

	for (const auto& annotation : this->m_annotations) {
		this->get_annotation_bytes(writer, annotation);
	}

	return writer->m_bytes;
}

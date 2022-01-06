#include "../../header/attributes/RuntimeVisibleAnnotationsAttribute.h"
#include "../../header/types/JavaClass.h"
#include "../../header/types/JavaAnnotation.h"

// works with RuntimeInvisibleAnnotationsAttribute as well.
RuntimeVisibleAnnotationsAttribute* RuntimeVisibleAnnotationsAttribute::parse() {
	const auto num_annotations = this->read_u2();

	for (auto i = 0; i < num_annotations; i++) {
		this->m_annotations.push_back(this->parse_annotation());
	}

	return this;
}

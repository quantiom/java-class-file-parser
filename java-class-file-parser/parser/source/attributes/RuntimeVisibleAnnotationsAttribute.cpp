#include "../../header/attributes/RuntimeVisibleAnnotationsAttribute.h"
#include "../../header/types/JavaClass.h"
#include "../../header/types/JavaAnnotation.h"

RuntimeVisibleAnnotationsAttribute* RuntimeVisibleAnnotationsAttribute::parse() {
	const auto num_annotations = this->read_u2();

	std::cout << "num: " << num_annotations << "\n";

	for (auto i = 0; i < num_annotations; i++) {
		const auto annotation = this->parse_annotation();
	}

	return this;
}

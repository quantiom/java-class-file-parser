#pragma once
#include <variant>

#include "JavaAttribute.h"
#include "CodeAttribute.h"
#include "ConstantValueAttribute.h"
#include "RuntimeAnnotationsAttribute.h"
#include "DeprecatedAttribute.h"

using ParsedAttribute = std::variant<JavaAttribute, DeprecatedAttribute, RuntimeAnnotationsAttribute, ConstantValueAttribute, CodeAttribute>;
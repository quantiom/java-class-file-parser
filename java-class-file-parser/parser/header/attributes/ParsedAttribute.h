#pragma once
#include <variant>

#include "JavaAttribute.h"
#include "CodeAttribute.h"
#include "ConstantValueAttribute.h"
#include "RuntimeAnnotationsAttribute.h"
#include "DeprecatedAttribute.h"
#include "UnparsedJavaAttribute.h"

using ParsedAttribute = std::variant<UnparsedJavaAttribute, DeprecatedAttribute, RuntimeVisibleAnnotationsAttribute, RuntimeInvisibleAnnotationsAttribute, ConstantValueAttribute, CodeAttribute>;
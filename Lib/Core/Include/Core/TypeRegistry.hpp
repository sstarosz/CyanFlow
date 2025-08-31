#ifndef CF_CORE_TYPEREGISTRY
#define CF_CORE_TYPEREGISTRY

#include <cstdint>

namespace cf::core {

using TypeHandle = std::uint64_t;
constexpr TypeHandle kInvalidTypeHandle = 0;

class TypeRegistry {

private:
};

} // namespace cf::core

#endif // CF_CORE_TYPEREGISTRY
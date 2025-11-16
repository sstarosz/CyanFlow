#ifndef CF_CORE_DATATYPES_HPP
#define CF_CORE_DATATYPES_HPP

#include <cstdint>
#include <string>

namespace cf::core {

using Bool = bool;

using Int32 = std::int32_t;
using UInt32 = std::uint32_t;
using Int64 = std::int64_t;
using UInt64 = std::uint64_t;

using Float = float;
using Double = double;

using String = std::string;

template <typename T>
concept IsCoreFundamentalType = std::same_as<T, Bool> || std::same_as<T, Int32> || std::same_as<T, UInt32> || std::same_as<T, Int64> || std::same_as<T, UInt64> || std::same_as<T, Float> || std::same_as<T, Double> || std::same_as<T, String>;

} // namespace cf::core

#endif // CF_CORE_DATATYPES_HPP
#ifndef CF_CORE_NODE
#define CF_CORE_NODE

#include "Core/TypeRegistry.hpp"

#include <cstdint>
#include <string>

namespace cf::core {
    
enum class Status {
    eOK,
    eError
};

template<typename NodeType>
concept NodeConcept = requires(NodeType node) {
    { node.compute() } -> std::same_as<Status>;
    { NodeType::initialize() } -> std::same_as<NodeDescriptor>;
    { node.getType() } -> std::same_as<TypeHandle>;
};



using NodeHandle = uint64_t;
static constexpr NodeHandle kInvalidNodeHandle = 0;

class Node {
public:
    virtual ~Node() = default;
    virtual Status compute() = 0;
    virtual TypeHandle getType() const = 0;

    NodeHandle getHandle() const { return m_handle; }

    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

private:
    NodeHandle m_handle = kInvalidNodeHandle;
    std::string m_name = "Unnamed Node";
};

} // namespace cf::core

#endif // CF_CORE_NODE
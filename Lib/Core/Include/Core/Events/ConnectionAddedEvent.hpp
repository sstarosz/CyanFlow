#ifndef CF_CORE_EVENTS_CONNECTIONADDEDEVENT_HPP
#define CF_CORE_EVENTS_CONNECTIONADDEDEVENT_HPP

#include "Core/Attribute.hpp" //TODO: Forward declare AttributeHandle?
#include "Core/EventBus.hpp"

namespace cf::core {

struct ConnectionAddedEvent : public Event {
    ConnectionAddedEvent(AttributeHandle fromAttr, AttributeHandle toAttr)
        : m_fromAttr(fromAttr), m_toAttr(toAttr)
    {
    }

    AttributeHandle m_fromAttr;
    AttributeHandle m_toAttr;
};

struct ConnectionRemovedEvent : public Event {
    ConnectionRemovedEvent(AttributeHandle fromAttr, AttributeHandle toAttr) 
        : fromAttr(fromAttr), toAttr(toAttr) {}
    
    AttributeHandle fromAttr;
    AttributeHandle toAttr;
};


} // namespace cf::core

#endif // CF_CORE_EVENTS_CONNECTIONADDEDEVENT_HPP
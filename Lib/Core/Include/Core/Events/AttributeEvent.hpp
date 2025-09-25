#ifndef CF_CORE_EVENTS_ATTRIBUTEEVENT_HPP
#define CF_CORE_EVENTS_ATTRIBUTEEVENT_HPP

#include "Core/Attribute.hpp" //TODO: Forward declare AttributeHandle?
#include "Core/EventBus.hpp"

namespace cf::core {

struct AttributeEvent : public Event {
    enum class AttributeMessage {
        eAttributeChanged,
        eAttributeConnected,
        eAttributeDisconnected
    };

    AttributeEvent(AttributeMessage msg, AttributeHandle handler) : m_message(msg),
        attributeHandle(handler)
    {
    }

    AttributeMessage m_message;
    AttributeHandle attributeHandle { kInvalidAttributeHandle };
};

} // namespace cf::core

#endif // CF_CORE_EVENTS_ATTRIBUTEEVENT_HPP
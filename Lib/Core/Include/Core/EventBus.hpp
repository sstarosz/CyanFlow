#ifndef CF_CORE_EVENTBUS_HPP
#define CF_CORE_EVENTBUS_HPP

#include "Core/TypeDescriptors.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <typeindex>

namespace cf::core {

using EventTypeHandle = uint64_t;
static constexpr EventTypeHandle kInvalidEventTypeHandle = 0;

struct Event {
    Event() = default;
    ~Event() = default;

    Event(const Event&) = default;
    Event(Event&&) = default;

    Event& operator=(const Event&) = default;
    Event& operator=(Event&&) = default;
};

struct EventBus {

    using SubscriptionId = size_t; // TODO: Move to TypeDescriptors.hpp?
    static constexpr SubscriptionId kInvalidSubscriptionId = 0;

    template <typename EventType>
    static SubscriptionId subscribe(std::function<void(const EventType&)> callback)
    {
        return getInstance().subscribeImpl<EventType>(std::move(callback));
    }

    template <typename EventType>
    static void publish(const EventType& event)
    {
        getInstance().publishImpl(event);
    }

    static void unsubscribe(SubscriptionId id)
    {
        getInstance().unsubscribeImpl(id);
    }

    static EventBus& getInstance()
    {
        static EventBus instance;
        return instance;
    }

    template <typename EventType>
    SubscriptionId subscribeImpl(std::function<void(const EventType&)> callback)
    {
        static_assert(
            std::is_base_of_v<Event, EventType>,
            "EventType must be derived from Event");

        SubscriptionId id = m_subscriptionId++;
        auto& callbacks = m_callbacks[typeid(EventType)];
        callbacks.emplace_back(id, [callback = std::move(callback)](const Event& event) {
            callback(static_cast<const EventType&>(event));
        });
        return id;
    }

    template <typename EventType>
    void publishImpl(const EventType& event)
    {
        auto it = m_callbacks.find(typeid(EventType));
        if (it != m_callbacks.end()) {
            for (const auto& callback : it->second) {
                callback.second(event);
            }
        }
    }

    void unsubscribeImpl(SubscriptionId id)
    {
        assert(id != kInvalidSubscriptionId && "Invalid subscription ID");

        for (auto& [type, callbacks] : m_callbacks) {
            std::erase_if(callbacks, [id](const CallbackPair& pair) { return pair.first == id; });
        }
    }

private:
    template <typename EventType>
    EventTypeHandle generateUniqueEventTypeHandle()
    {
        static EventTypeHandle currentHandle = 0;
        return ++currentHandle;
    }

    using TypeErasedCallback = std::function<void(const Event&)>;
    using CallbackPair = std::pair<SubscriptionId, TypeErasedCallback>;
    std::unordered_map<std::type_index, std::vector<CallbackPair>> m_callbacks;
    SubscriptionId m_subscriptionId { 0 };
};

} // namespace cf::core

#endif // CF_CORE_EVENTBUS_HPP
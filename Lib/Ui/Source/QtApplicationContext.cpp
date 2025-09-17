#include "QtApplicationContext.hpp"

namespace cf::ui {

void QtApplicationContext::setupEventSubscriptions()
{
    m_subscriptions.push_back(core::EventBus::subscribe<core::ConnectionAddedEvent>([this](const core::ConnectionAddedEvent& event) {
        QMetaObject::invokeMethod(this, [this, event]() { emit connectionAdded(event); }, Qt::QueuedConnection);
    }));
}

} // namespace cf::ui
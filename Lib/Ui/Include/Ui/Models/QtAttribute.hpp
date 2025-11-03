#ifndef CF_UI_MODELS_QTATTRIBUTE_HPP
#define CF_UI_MODELS_QTATTRIBUTE_HPP

#include "Core/Attribute.hpp"
#include "Core/EventBus.hpp"

#include <QObject>
#include <QVariant>

namespace cf::ui {

class QtAttribute : public QObject {
    Q_OBJECT
public:
    explicit QtAttribute(std::shared_ptr<core::Attribute> attribute, QObject* parent = nullptr);

    ~QtAttribute() override;

    bool isInput() const;

    bool isOutput() const;

    bool isInOut() const;

    QString getName() const;

    std::shared_ptr<core::Attribute> getAttribute() const;

    core::AttributeDescriptor getAttributeDescriptor() const;
    QVariant getValue() const;

signals:
    void valueChanged();

private:
    std::shared_ptr<core::Attribute> m_attribute;
    core::EventBus::SubscriptionId m_eventSubscriptionId { core::EventBus::kInvalidSubscriptionId };
};

} // namespace cf::ui

#endif // CF_UI_MODELS_QTATTRIBUTE_HPP
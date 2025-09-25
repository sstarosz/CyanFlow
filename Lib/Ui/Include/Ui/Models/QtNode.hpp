#ifndef CF_UI_MODELS_QTNODE_HPP
#define CF_UI_MODELS_QTNODE_HPP

#include "Core/Node.hpp"

#include <QList>
#include <QObject>
#include <QPointer>
#include <QtGlobal>

namespace cf::ui {
class QtAttribute;
class QtApplicationContext;

class QtNode : public QObject {
    Q_OBJECT
public:
    explicit QtNode(std::shared_ptr<core::Node> node,
        QtApplicationContext& appContext,
        QObject* parent = nullptr);

    QString getName() const;
    void setName(const QString& name);

    const QList<QPointer<QtAttribute>>& getAttributes() const;

    std::shared_ptr<core::Node> getNode() const;

private:
    void loadAttributes();

    std::shared_ptr<core::Node> m_node;
    QtApplicationContext& m_appContext;
    QList<QPointer<QtAttribute>> m_attributes;
};

} // namespace cf::ui

inline size_t qHash(const QPointer<cf::ui::QtNode>& ptr, size_t seed = 0) noexcept
{
    return qHash(ptr.data(), seed);
}

#endif // CF_UI_MODELS_QTNODE_HPP
#include "Models/QtNode.hpp"

#include "Ui/Models/QtAttribute.hpp"
#include "Ui/QtApplicationContext.hpp"

#include <QHash>

namespace cf::ui {

QtNode::QtNode(std::shared_ptr<core::Node> node,
    QtApplicationContext& appContext,
    QObject* parent)
    : QObject(parent)
    , m_node(node)
    , m_appContext(appContext)
    , m_attributes()
{
    loadAttributes();
}

QString QtNode::getName() const { return QString::fromStdString(m_node->getName()); }
void QtNode::setName(const QString& name) { m_node->setName(name.toStdString()); }

const QList<QPointer<QtAttribute>>& QtNode::getAttributes() const { return m_attributes; }
std::shared_ptr<core::Node> QtNode::getNode() const { return m_node; }

void QtNode::loadAttributes()
{
    auto attributes = m_appContext.getActiveScene()->getNodeAttributes(m_node);
    for (const auto& attr : attributes) {
        auto qtAttr = new QtAttribute(attr, this);
        m_attributes.append(qtAttr);
    }
}

} // namespace cf::ui
#ifndef CF_UI_SELECTIONMANAGER_HPP
#define CF_UI_SELECTIONMANAGER_HPP

#include "Core/Scene.hpp"

#include "Ui/Models/QtNode.hpp"

#include <QObject>
#include <QSet>
#include <QPointer>
namespace cf::ui
{

class SelectionManager : public QObject
{
    Q_OBJECT
public:
    SelectionManager() = default;


    void addNodeToSelection(QPointer<QtNode> qtNode)
    {
        if (m_selectedNodes.contains(qtNode))
            return;
        m_selectedNodes.insert(qtNode);
        emit selectionChanged(m_selectedNodes);
    }


    void setSelection(QSet<QPointer<QtNode>> nodes)
    {
        m_selectedNodes = std::move(nodes);
        emit selectionChanged(m_selectedNodes);
    }

signals:
    void selectionChanged(const QSet<QPointer<QtNode>>& selectedQtNodes);


private:
    QSet<QPointer<QtNode>> m_selectedNodes;
    //QSet<QPointer<QtAttribute>> m_selectedAttributes;
};

}// namespace cf::ui

#endif // CF_UI_SELECTIONMANAGER_HPP
#ifndef CF_UI_SELECTIONMANAGER_HPP
#define CF_UI_SELECTIONMANAGER_HPP

#include <QObject>
#include "Core/Scene.hpp"

namespace cf::ui
{

class SelectionManager : public QObject
{
    Q_OBJECT
public:
    static SelectionManager& getInstance()
    {
        static SelectionManager instance;
        return instance;
    }

    void addNode(std::shared_ptr<core::Node> node)
    {
        selectedNodes.push_back(node);
        emit selectionChanged(selectedNodes);
    }

    void setSelection(const std::vector<std::shared_ptr<core::Node>>& nodes)
    {
        selectedNodes = nodes;
        emit selectionChanged(selectedNodes);
    }

signals:
    void selectionChanged(const std::vector<std::shared_ptr<core::Node>>& selectedNodes);


private:
    SelectionManager() = default;


    std::vector<std::shared_ptr<core::Node>> selectedNodes;
};

}// namespace cf::ui

#endif // CF_UI_SELECTIONMANAGER_HPP
#ifndef CF_UI_NODEEDITOR_HPP
#define CF_UI_NODEEDITOR_HPP


#include "Core/Scene.hpp"
#include <QWidget>

namespace cf::ui
{
class NodeEditor : public QWidget
    {
        Q_OBJECT
      public:
        NodeEditor(std::shared_ptr<core::Scene> scene, QWidget* parent = nullptr);

      private:
        std::shared_ptr<core::Scene> m_scene;
    };
} // namespace cf::ui

#endif // CF_UI_NODEEDITOR_HPP
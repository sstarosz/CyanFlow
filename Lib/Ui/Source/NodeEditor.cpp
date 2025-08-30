#include "NodeEditor.hpp"

#include <QHBoxLayout>
#include <QLabel>


namespace cf::ui
{
    NodeEditor::NodeEditor(std::shared_ptr<core::Scene> scene, QWidget* parent):
        QWidget(parent), m_scene(scene)
    {
        QHBoxLayout* nodeEditorLayout = new QHBoxLayout(this);
        nodeEditorLayout->setContentsMargins(0, 0, 0, 0);
        nodeEditorLayout->setSpacing(0);

        QLabel* titleLabel = new QLabel("Node Editor", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; padding: 10px; background-color: #323232ff;");
        nodeEditorLayout->addWidget(titleLabel);


        setLayout(nodeEditorLayout);
    }


} // namespace cf::ui
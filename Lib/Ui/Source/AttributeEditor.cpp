#include "AttributeEditor.hpp"
#include "SelectionManager.hpp"
#include "Core/TypeRegistry.hpp"

#include <QLabel>
#include <QFormLayout>

namespace cf::ui {

AttributeEditor::AttributeEditor(QWidget* parent):
    QWidget(parent)
{
    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(5, 5, 5, 5);
    m_rootLayout->setSpacing(5);

    m_attributesContainer = new QLabel("No node selected", this);
    m_rootLayout->addWidget(m_attributesContainer);


    connect(&SelectionManager::getInstance(), &SelectionManager::selectionChanged,
            this, &AttributeEditor::onSelectionChanged);
}

AttributeEditor::~AttributeEditor() = default;

void AttributeEditor::clearAttributes()
{
    if(!m_attributesContainer)
    {
        return;
    }

    m_attributesContainer->deleteLater();
    m_attributesContainer = nullptr; 
}

void AttributeEditor::displayNodeAttributes(std::shared_ptr<core::Node> node)
{
    Q_UNUSED(node);
    clearAttributes();

    QWidget* panel = new QWidget(this);
    auto* form = new QFormLayout(panel);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // Example fields – extend with real attributes from your core::Scene
    form->addRow(new QLabel("Name:"), new QLabel(QString::fromStdString(node->getName())));

    core::NodeDescriptor desc = core::TypeRegistry::getNodeDescriptor(node->getType());
    for (const auto& attrDesc : desc.attributes) {
        form->addRow(new QLabel(QString::fromStdString(attrDesc.name)), new QLabel("Value")); // Placeholder for value
    }


    

    m_rootLayout->addWidget(panel);
    m_attributesContainer = panel;
}

void AttributeEditor::onSelectionChanged(const std::vector<std::shared_ptr<core::Node>>& selectedNodes)
{
    // Handle selection change
    // For example, update the UI to show attributes of the selected nodes
    if(selectedNodes.size() == 1)
    {
        displayNodeAttributes(selectedNodes[0]);
    }
}

} // namespace cf::ui

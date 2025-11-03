#include "AttributeEditor.hpp"
#include "Core/TypeRegistry.hpp"
#include "SelectionManager.hpp"
#include "Ui/CollapsibleWidget.hpp"
#include "Ui/Models/QtAttribute.hpp"
#include "Ui/Models/QtNode.hpp"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>

namespace {

QString attributeValueToString(const cf::ui::QtAttribute& attribute)
{
    QVariant value = attribute.getValue();
    if (value.canConvert<double>()) {
        return QString::number(value.toDouble());
    } else {
        return "Invalid";
    }
}

QWidget* createWidgetForAttribute(const cf::ui::QtAttribute& attribute)
{

    auto typeHandle = attribute.getAttribute()->getAttributeDescriptor().typeHandle;
    if (typeHandle == cf::core::TypeRegistry::getTypeHandle<float>()) {
        if (attribute.isInput()) {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox();
            spinBox->setValue(attribute.getValue().toDouble());
            spinBox->setSingleStep(0.1);
            spinBox->setDecimals(4);

            QObject::connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&attribute](double newValue) {
                attribute.getAttribute()->setValue(static_cast<float>(newValue));
            });

            QObject::connect(&attribute, &cf::ui::QtAttribute::valueChanged, [spinBox, &attribute]() {
                spinBox->setValue(attribute.getValue().toDouble());
            });

            return spinBox;
        } else {
            QLabel* label = new QLabel(QString::number(attribute.getValue().toDouble()));

            QObject::connect(&attribute, &cf::ui::QtAttribute::valueChanged, [label, &attribute]() {
                QVariant value = attribute.getValue();
                QString text;
                if (value.canConvert<double>()) {
                    text = QString::number(value.toDouble());
                } else {
                    text = "Invalid";
                }

                label->setText(text);
            });

            return label;
        }
    }

    // todo add more types

    return new QLabel("Unsupported Type");
}
}

namespace cf::ui {

AttributeEditor::AttributeEditor(QtApplicationContext& appContext, QWidget* parent)
    : QWidget(parent)
    , m_appContext(appContext)
    , m_rootLayout(new QVBoxLayout(this))
    , m_attributeEditorPanel(new QWidget(this))
    , m_tabWidget(new QTabWidget(m_attributeEditorPanel))
{
    m_rootLayout->setContentsMargins(5, 5, 5, 5);
    m_rootLayout->setSpacing(5);
    m_rootLayout->addWidget(m_attributeEditorPanel);

    setLayout(m_rootLayout);

    QVBoxLayout* panelLayout = new QVBoxLayout(m_attributeEditorPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->addWidget(new QLabel("Attribute Editor", m_attributeEditorPanel));
    panelLayout->addWidget(m_tabWidget);

    // Add test tab
    m_tabWidget->addTab(new QLabel("Test Tab Content"), "Test Tab");

    connect(&m_appContext.selectionManager(), &SelectionManager::selectionChanged,
        this, &AttributeEditor::onSelectionChanged);
}

AttributeEditor::~AttributeEditor() = default;

void AttributeEditor::clearAttributes()
{
    if (!m_tabWidget) {
        return;
    }

    m_tabWidget->clear();
}

void AttributeEditor::displayNodeAttributes(const QPointer<QtNode>& node)
{
    Q_UNUSED(node);
    clearAttributes();

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget(scrollArea);
    QVBoxLayout* mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setAlignment(Qt::AlignTop);

    QVBoxLayout* attributeGroupLayout = new QVBoxLayout();

    for (const auto& attribute : node->getAttributes()) {
        QFormLayout* formLayout = new QFormLayout();
        spdlog::debug("Adding attribute to UI: {} (handle: {}, ptr: {})",
            attribute->getName().toStdString(),
            attribute->getAttribute()->getHandle(),
            static_cast<void*>(attribute->getAttribute().get()));

        formLayout->addRow(new QLabel(attribute->getName() + ":"), createWidgetForAttribute(*attribute));
        attributeGroupLayout->addLayout(formLayout);
    }

    CollapsibleWidget* collapsible = new CollapsibleWidget("Attributes", this);
    collapsible->setContentLayout(attributeGroupLayout);
    mainLayout->addWidget(collapsible);

    scrollArea->setWidget(scrollContent);
    m_tabWidget->addTab(scrollArea, node ? node->getName() : "No Node");
}

void AttributeEditor::onSelectionChanged(const QSet<QPointer<QtNode>>& selectedNodes)
{
    // Handle selection change
    // For example, update the UI to show attributes of the selected nodes
    if (selectedNodes.size() == 1) {
        displayNodeAttributes(selectedNodes.values().first());
    }
}

} // namespace cf::ui

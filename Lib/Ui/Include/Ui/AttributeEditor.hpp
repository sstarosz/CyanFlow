#ifndef CF_UI_ATTRIBUTEEDITOR_HPP
#define CF_UI_ATTRIBUTEEDITOR_HPP

#include "Core/Scene.hpp"
#include <QWidget>
#include <QVBoxLayout>

namespace cf::ui
{

class AttributeEditor : public QWidget
{
    Q_OBJECT
public:
    AttributeEditor(QWidget* parent = nullptr);
    ~AttributeEditor() override;

public slots:
    void onSelectionChanged(const std::vector<std::shared_ptr<core::Node>>& selectedNodes);

private:
    void clearAttributes();
    void displayNodeAttributes(std::shared_ptr<core::Node> node);
    

    QVBoxLayout* m_rootLayout = nullptr;
    QWidget* m_attributesContainer = nullptr;
};



}// namespace cf::ui

#endif // CF_UI_ATTRIBUTEEDITOR_HPP
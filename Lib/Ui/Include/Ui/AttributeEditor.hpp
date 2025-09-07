#ifndef CF_UI_ATTRIBUTEEDITOR_HPP
#define CF_UI_ATTRIBUTEEDITOR_HPP

#include <QWidget>

namespace cf::ui
{

class AttributeEditor : public QWidget
{
    Q_OBJECT
public:
    AttributeEditor(QWidget* parent = nullptr);
    ~AttributeEditor() override;

public slots:
    void onSelectionChanged();
    
};



}// namespace cf::ui

#endif // CF_UI_ATTRIBUTEEDITOR_HPP
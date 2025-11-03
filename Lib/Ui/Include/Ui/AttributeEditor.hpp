#ifndef CF_UI_ATTRIBUTEEDITOR_HPP
#define CF_UI_ATTRIBUTEEDITOR_HPP

#include "Core/Scene.hpp"
#include "Ui/QtApplicationContext.hpp"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui {

class AttributeEditor : public QWidget {
    Q_OBJECT
public:
    AttributeEditor(QtApplicationContext& appContext, QWidget* parent = nullptr);
    ~AttributeEditor() override;

public slots:
    void onSelectionChanged(const QSet<QPointer<QtNode>>& selectedNodes);

private:
    void clearAttributes();
    void displayNodeAttributes(const QPointer<QtNode>& node);

    QtApplicationContext& m_appContext;

    QVBoxLayout* m_rootLayout = nullptr;
    QWidget* m_attributeEditorPanel = nullptr;
    QTabWidget* m_tabWidget = nullptr;
};

// AttributeEditorWidget

} // namespace cf::ui

#endif // CF_UI_ATTRIBUTEEDITOR_HPP
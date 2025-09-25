#include "MainWindow.hpp"
#include "AttributeEditor.hpp"
#include "NodeEditor.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>

#include <spdlog/spdlog.h>

namespace cf::ui {
    
MainWindow::MainWindow(QtApplicationContext& appContext,
    QWidget* parent,
    Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_appContext(appContext)
{
    spdlog::info("MainWindow::MainWindow()");

    setContentsMargins(0, 0, 0, 0);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topBarWidget = new QWidget(this);
    QHBoxLayout* topBarLayout = new QHBoxLayout(topBarWidget);
    topBarLayout->setContentsMargins(0, 0, 0, 0);
    topBarLayout->addWidget(new QLabel("Left", this));
    topBarLayout->addWidget(new QLabel("Center", this));
    topBarLayout->addWidget(new QLabel("Right", this));

    // Node editor

    // Splitter between top bar and node editor
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    splitter->setStyleSheet("QSplitter::handle { background-color: lightgray; }");
    splitter->addWidget(topBarWidget);

    ui::NodeEditor* nodeEditor = new ui::NodeEditor(appContext, this);
    ui::AttributeEditor* attributeEditor = new ui::AttributeEditor(appContext, this);
    QSplitter* nodeEditorSpliter = new QSplitter(Qt::Horizontal, this);
    nodeEditorSpliter->setStyleSheet("QSplitter::handle { background-color: lightgray; }");
    nodeEditorSpliter->addWidget(nodeEditor);
    nodeEditorSpliter->addWidget(attributeEditor);
    QList<int> nodeEditorSizes;
    nodeEditorSizes << 300 << 100;
    nodeEditorSpliter->setSizes(nodeEditorSizes);

    splitter->addWidget(nodeEditorSpliter);

    QList<int> sizes;
    sizes << 100 << 100;
    splitter->setSizes(sizes);

    centralLayout->addWidget(splitter);
    setCentralWidget(centralWidget);
    setWindowState(Qt::WindowMaximized);
}

} // namespace cf::ui
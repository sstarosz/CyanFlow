#ifndef CF_UI_MAINWINDOW_HPP
#define CF_UI_MAINWINDOW_HPP

#include "Ui/QtApplicationContext.hpp"
#include <QMainWindow>

namespace cf::ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QtApplicationContext& appContext,
        QWidget* parent = nullptr,
        Qt::WindowFlags flags = {});

private:
    QtApplicationContext& m_appContext;
};

} // namespace cf::ui

#endif // CF_UI_MAINWINDOW_HPP
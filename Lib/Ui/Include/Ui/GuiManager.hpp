#ifndef CF_UI_GUIMANAGER_HPP
#define CF_UI_GUIMANAGER_HPP

#include "Framework/ApplicationContext.hpp"
#include "Ui/MainWindow.hpp"
#include "Ui/QtApplicationContext.hpp"

namespace cf::ui {
class GuiManager {
public:
    GuiManager(framework::ApplicationContext& appContext);

public:
    void show();

private:
    QtApplicationContext m_qtAppContext;
    ui::MainWindow m_mainWindow;
};

} // namespace cf::ui

#endif // CF_UI_GUIMANAGER_HPP
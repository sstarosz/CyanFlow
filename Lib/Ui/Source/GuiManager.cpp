#include "GuiManager.hpp"
#include "Framework/ApplicationContext.hpp"

namespace cf::ui {

GuiManager::GuiManager(framework::ApplicationContext& appContext)
    : m_qtAppContext(appContext)
    , m_mainWindow(m_qtAppContext)
{
}

void GuiManager::show()
{
    m_mainWindow.show();
}

} // namespace cf::ui

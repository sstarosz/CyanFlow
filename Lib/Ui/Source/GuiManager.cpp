#include "GuiManager.hpp"

namespace cf::ui
{
	GuiManager::GuiManager(std::shared_ptr<core::Scene> scene):
        m_scene(scene),
        m_mainWindow(scene)
	{
	}

	void GuiManager::show()
	{
        m_mainWindow.show();
    }

} // namespace cf::ui

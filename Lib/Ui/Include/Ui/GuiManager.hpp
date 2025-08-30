#ifndef CF_UI_GUIMANAGER_HPP
#define CF_UI_GUIMANAGER_HPP

#include "Core/Scene.hpp"
#include "Ui/MainWindow.hpp"

#include <memory>
	
namespace cf::ui
{
class GuiManager
	{
	  public:
		GuiManager(std::shared_ptr<core::Scene> scene);

	  public:
		void show();

	  private:
		ui::MainWindow m_mainWindow;
		std::shared_ptr<core::Scene> m_scene;
	};

} // namespace cf::ui

#endif // CF_UI_GUIMANAGER_HPP
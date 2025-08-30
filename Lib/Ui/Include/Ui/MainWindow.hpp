#ifndef CF_UI_MAINWINDOW_HPP
#define CF_UI_MAINWINDOW_HPP

#include "Core/Scene.hpp"
#include <QMainWindow>

namespace cf::ui
{

	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	  public:
		explicit MainWindow(std::shared_ptr<core::Scene> scene,
							QWidget* parent = nullptr,
							Qt::WindowFlags flags = {});

	  	private:
		std::shared_ptr<core::Scene> m_scene;
	};

} // namespace cf::ui

#endif // CF_UI_MAINWINDOW_HPP
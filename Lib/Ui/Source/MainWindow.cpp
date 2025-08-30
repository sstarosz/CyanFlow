#include "MainWindow.hpp"
#include "NodeEditor.hpp"

#include <QHBoxLayout>
#include <QLabel>

#include <spdlog/spdlog.h>

namespace cf::ui
{
	MainWindow::MainWindow(std::shared_ptr<core::Scene> scene,
						   QWidget* parent,
						   Qt::WindowFlags flags):
		QMainWindow(parent, flags)
	{
		spdlog::info("MainWindow::MainWindow()");

		setContentsMargins(0, 0, 0, 0);

		QWidget* centralWidget = new QWidget(this);
		QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
		centralLayout->setContentsMargins(0, 0, 0, 0);

		QHBoxLayout* topBarLayout = new QHBoxLayout();
		topBarLayout->addWidget(new QLabel("Left", this));
		topBarLayout->addWidget(new QLabel("Center", this));
		topBarLayout->addWidget(new QLabel("Right", this));

		centralLayout->addLayout(topBarLayout);
		centralLayout->addWidget(new ui::NodeEditor(scene, this));

		setCentralWidget(centralWidget);

		setWindowState(Qt::WindowMaximized);
	}

} // namespace cf::ui
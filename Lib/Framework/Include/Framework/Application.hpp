#ifndef CF_FRAMEWORK_APPLICATION
#define CF_FRAMEWORK_APPLICATION

#include "Core/Scene.hpp"
#include "Core/Document.hpp"
#include "Framework/ApplicationContext.hpp"
#include "Ui/GuiManager.hpp"

#include <QApplication>

namespace cf::framework {


class Application {
public:
    Application(int argc, char* argv[]);
    int run();

private:
    QApplication m_app;

    ApplicationContext m_appContext;

    ui::GuiManager m_guiManager;
};

} // namespace cf::framework

#endif // CF_FRAMEWORK_APPLICATION

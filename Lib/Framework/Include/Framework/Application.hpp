#ifndef CF_FRAMEWORK_APPLICATION
#define CF_FRAMEWORK_APPLICATION

#include "Core/Scene.hpp"
#include "Core/Document.hpp"
#include "Ui/GuiManager.hpp"

#include <QApplication>

namespace cf::framework {

class ApplicationContext{
public:


    std::shared_ptr<core::Document> m_currentDocument;
};


class Application {
public:
    Application(int argc, char* argv[]);
    int run();

private:
    QApplication m_app;

    std::shared_ptr<core::Scene> m_scene;
    std::shared_ptr<core::Document> m_document;

    ui::GuiManager m_guiManager;
};

} // namespace cf::framework

#endif // CF_FRAMEWORK_APPLICATION

#include "Application.hpp"

#include "Core/Nodes/AddNode.hpp"
#include <spdlog/spdlog.h>

namespace cf::framework {
Application::Application(int argc, char* argv[])
    : m_app(argc, argv)
    , m_scene(std::make_shared<core::Scene>())
    , m_guiManager(m_scene)
{
}

int Application::run()
{
    spdlog::info("Application::start()");

    m_scene->addNode(std::make_unique<core::AddNode>());

    m_guiManager.show();

    return m_app.exec();
}

} // namespace cf::framework

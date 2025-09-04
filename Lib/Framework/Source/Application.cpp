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

void registerTypes()
{
    core::TypeRegistry::registerType<float>("float");
}

void registerNodeTypes()
{
    core::TypeRegistry::registerNodeType<core::AddNode>();
}


int Application::run()
{
    spdlog::info("Application::start()");

    registerTypes();
    registerNodeTypes();



    m_scene->addNode(std::make_unique<core::AddNode>());

    m_guiManager.show();

    return m_app.exec();
}

} // namespace cf::framework

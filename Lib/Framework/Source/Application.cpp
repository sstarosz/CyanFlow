#include "Application.hpp"

#include "Core/DataTypes.hpp"
#include "Core/Events/AttributeEvent.hpp"
#include "Core/Events/ConnectionAddedEvent.hpp"
#include "Core/Nodes/AddNode.hpp"
#include <spdlog/spdlog.h>

namespace cf::framework {
Application::Application(int argc, char* argv[])
    : m_app(argc, argv)
    , m_guiManager(m_appContext)
{
    spdlog::set_level(spdlog::level::debug);
}

void registerTypes()
{
    core::TypeRegistry::registerType<core::Bool>();

    core::TypeRegistry::registerType<core::Int32>();
    core::TypeRegistry::registerType<core::UInt32>();
    core::TypeRegistry::registerType<core::Int64>();
    core::TypeRegistry::registerType<core::UInt64>();

    core::TypeRegistry::registerType<core::Float>();
    core::TypeRegistry::registerType<core::Double>();

    core::TypeRegistry::registerType<core::String>();
}

void registerNodeTypes()
{
    core::TypeRegistry::registerNodeType<core::AddNode>();
}

void registerEventTypes()
{
    core::TypeRegistry::registerEventType<core::AttributeEvent>("AttributeEvent", "Attribute");
    core::TypeRegistry::registerEventType<core::ConnectionAddedEvent>("ConnectionAddedEvent", "Connection");
    core::TypeRegistry::registerEventType<core::ConnectionRemovedEvent>("ConnectionRemovedEvent", "Connection");
}

int Application::run()
{
    spdlog::info("Application::start()");

    registerTypes();
    registerNodeTypes();

    m_appContext.createNewDocument();
    m_appContext.getCurrentDocument()->createNewScene();

    auto nodeA = m_appContext.getActiveScene()->addNode(std::make_unique<core::AddNode>());
    auto nodeB = m_appContext.getActiveScene()->addNode(std::make_unique<core::AddNode>());

    m_appContext.getActiveScene()->connect(nodeA, nodeA->outputs.result, nodeB, nodeB->inputs.input1);

    m_guiManager.show();

    return m_app.exec();
}

} // namespace cf::framework

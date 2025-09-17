#ifndef CF_CORE_COMMANDS_ADDCONNECTIONCOMMAND_HPP
#define CF_CORE_COMMANDS_ADDCONNECTIONCOMMAND_HPP

#include "Core/Command.hpp"
#include "Core/Scene.hpp"
#include "Core/Events/ConnectionAddedEvent.hpp"
#include <memory>



namespace cf::core {
class AddConnectionCommand : public Command {
public:
    AddConnectionCommand(std::shared_ptr<Scene> scene, 
                        AttributeHandle fromAttr, 
                        AttributeHandle toAttr)
        : m_scene(scene), m_fromAttr(fromAttr), m_toAttr(toAttr) {}
    
    void execute() override {
        if (!m_connectionCreated) {
            m_scene->addConnection(m_fromAttr, m_toAttr);
            m_connectionCreated = true;

            EventBus::publish(ConnectionAddedEvent(m_fromAttr, m_toAttr));
        } else {
            // Restore connection if it was previously undone
            m_scene->addConnection(m_fromAttr, m_toAttr);
            
            EventBus::publish(ConnectionAddedEvent(m_fromAttr, m_toAttr));
        }
    }
    
    void undo() override {
        m_scene->removeConnection(m_fromAttr, m_toAttr);
        EventBus::publish(ConnectionRemovedEvent(m_fromAttr, m_toAttr));
    }
    
private:
    std::shared_ptr<Scene> m_scene;
    AttributeHandle m_fromAttr;
    AttributeHandle m_toAttr;
    bool m_connectionCreated = false;
};

}   // namespace cf::core

#endif // CF_CORE_COMMANDS_ADDCONNECTIONCOMMAND_HPP
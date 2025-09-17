#ifndef CF_FRAMEWORK_APPLICATIONCONTEXT_HPP
#define CF_FRAMEWORK_APPLICATIONCONTEXT_HPP

#include "Core/Document.hpp"

namespace cf::framework {

class ApplicationContext{
public:
    
    //Document
    void createNewDocument() {
        m_currentDocument = std::make_shared<core::Document>();
    }

    std::shared_ptr<core::Document> getCurrentDocument() const {
            return m_currentDocument;
    }


    //Scene
    std::shared_ptr<core::Scene> getActiveScene() const {
        if (!m_currentDocument) {
            throw std::runtime_error("No active document set in ApplicationContext");
        }

        return m_currentDocument->getScene();
    }

    //Undo
    core::UndoStack& undoStack() { 
        return getCurrentDocument()->getUndoStack(); 
    }
    const core::UndoStack& undoStack() const { 
        return getCurrentDocument()->getUndoStack(); 
    }


    template<typename CommandType, typename... Args>
    void execute(Args&&... args) {
        if (!m_currentDocument) {
            throw std::runtime_error("No active document set in ApplicationContext");
        }

        auto command = std::make_unique<CommandType>(std::forward<Args>(args)...);
       
        m_currentDocument->getUndoStack().push(std::move(command));
    }


    std::shared_ptr<core::Document> m_currentDocument;
};

} // namespace cf::framework

#endif // CF_FRAMEWORK_APPLICATIONCONTEXT_HPP

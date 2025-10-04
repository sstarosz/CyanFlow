#ifndef CF_CORE_DOCUMENT_HPP
#define CF_CORE_DOCUMENT_HPP

#include "Core/Scene.hpp"
#include "Core/UndoStack.hpp"

namespace cf::core {
class Document {
public:

    void createNewScene() { m_scene = std::make_shared<Scene>(); }
    std::shared_ptr<Scene> getScene() const { return m_scene; }


    UndoStack& getUndoStack() { return m_undoRedoManager; }

private:
    std::shared_ptr<Scene> m_scene;
    UndoStack m_undoRedoManager;
    //SelectionManager m_selectionManager;
};

} // namespace cf::core

#endif // CF_CORE_DOCUMENT_HPP
#ifndef CF_CORE_UNDOSTACK_HPP
#define CF_CORE_UNDOSTACK_HPP

#include "Core/Command.hpp"
#include <memory>
#include <stack>

namespace cf::core {

class UndoStack {
public:
    void push(std::unique_ptr<Command> command);
    void undo();
    void redo();
    void clear();

    bool canUndo() const;
    bool canRedo() const;

private:
    std::stack<std::unique_ptr<Command>> m_undoStack;
    std::stack<std::unique_ptr<Command>> m_redoStack;
};

} // namespace cf::core

#endif // CF_CORE_UNDOSTACK_HPP
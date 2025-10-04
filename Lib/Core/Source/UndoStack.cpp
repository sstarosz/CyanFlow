#include  "UndoStack.hpp"

namespace cf::core {

    void UndoStack::push(std::unique_ptr<Command> command)
    {
        while (!m_redoStack.empty()) {
            m_redoStack.pop();
        }

        command->execute();

        m_undoStack.push(std::move(command));
    }

    void UndoStack::undo()
    {
        if (m_undoStack.empty()) {
            return;
        }

        auto command = std::move(m_undoStack.top());
        m_undoStack.pop();

        command->undo();
        m_redoStack.push(std::move(command));
    }

    void UndoStack::redo()
    {
        if (m_redoStack.empty()) {
            return;
        }

        auto command = std::move(m_redoStack.top());
        m_redoStack.pop();

        command->execute();
        m_undoStack.push(std::move(command));
    }

    void UndoStack::clear()
    {
        while (!m_undoStack.empty()) {
            m_undoStack.pop();
        }
        while (!m_redoStack.empty()) {
            m_redoStack.pop();
        }
    }

    bool UndoStack::canUndo() const { return !m_undoStack.empty(); }
    bool UndoStack::canRedo() const { return !m_redoStack.empty(); }
} // namespace cf::core
#ifndef CF_CORE_COMMAND
#define CF_CORE_COMMAND

#include <memory>
#include <stack>

namespace cf::core {
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};


class UndoStack {
public:
    UndoStack() = default;
    ~UndoStack() = default;
    
    void push(std::unique_ptr<Command> command)
    {
        while (!m_redoStack.empty()) {
            m_redoStack.pop();
        }

        command->execute();

        m_undoStack.push(std::move(command));
    }

    void undo()
    {
        if (m_undoStack.empty()) {
            return;
        }

        auto command = std::move(m_undoStack.top());
        m_undoStack.pop();

        command->undo();
        m_redoStack.push(std::move(command));
    }

    void redo()
    {
        if (m_redoStack.empty()) {
            return;
        }

        auto command = std::move(m_redoStack.top());
        m_redoStack.pop();

        command->execute();
        m_undoStack.push(std::move(command));
    }

    void clear()
    {
        while (!m_undoStack.empty()) {
            m_undoStack.pop();
        }
        while (!m_redoStack.empty()) {
            m_redoStack.pop();
        }
    }
    
    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }
    
private:
    std::stack<std::unique_ptr<Command>> m_undoStack;
    std::stack<std::unique_ptr<Command>> m_redoStack;
};

} // namespace cf::core

#endif // CF_CORE_COMMAND

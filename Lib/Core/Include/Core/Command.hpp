#ifndef CF_CORE_COMMAND_HPP
#define CF_CORE_COMMAND_HPP

namespace cf::core {
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

} // namespace cf::core

#endif // CF_CORE_COMMAND_HPP

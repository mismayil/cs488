#pragma once

#include "Command.hpp"
#include "JointNode.hpp"

class JointCommand : public Command {
    JointNode &node;

    public:
        JointCommand(JointNode &node, glm::mat4 T);
        virtual void redo() override;
        virtual void undo() override;
};

#pragma once

#include "Command.hpp"
#include "GeometryNode.hpp"

class GeometryCommand : public Command {
    GeometryNode &node;

    public:
        GeometryCommand(GeometryNode &node, glm::mat4 T);
        virtual void redo() override;
        virtual void undo() override;
};

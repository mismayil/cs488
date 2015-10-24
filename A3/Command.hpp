#pragma once
#include <glm/gtc/matrix_transform.hpp>

class Command {
    glm::mat4 T;
    public:
        Command(glm::mat4 T);
        virtual void redo()=0;
        virtual void undo()=0;
};

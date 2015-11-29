#pragma once

#include <glm/glm.hpp>

class Ray {
    public:
        glm::vec3 o;  // ray origin
        glm::vec3 d;  // ray direction
        Ray(glm::vec3 o, glm::vec3 d);
};

#pragma once

#include <glm/glm.hpp>

class Material;

// intersection info
class TAO {
    public:
        double tao;
        bool hit;
        glm::vec3 n;
        Material *material;
        TAO();
        TAO(double tao, bool hit, glm::vec3 n);
};

#pragma once

#include <glm/glm.hpp>

class Material;

// intersection info
class TAO {
    public:
        double taomin, taomax;
        glm::vec3 nmin, nmax;
        Material *materialmin;
        Material *materialmax;
        TAO();
        TAO(double taomin, double taomax, glm::vec3 nmin, glm::vec3 nmax);
        TAO(double taomin, double taomax, glm::vec3 nmin, glm::vec3 nmax, Material *mmin, Material *mmax);
};

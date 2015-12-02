#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include "Light.hpp"

class AreaLight : public Light {
public:
    double width;
    double height;
    AreaLight(glm::vec3 pos, double w, double h, glm::vec3 color, glm::vec3 falloff);
    double getArea();
    std::vector<softRay> getRays(glm::vec3 point, double tao);
};

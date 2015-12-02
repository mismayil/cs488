#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include <vector>
#include "util.hpp"

class Light {
public:
    glm::vec3 colour;
    glm::vec3 position;
    glm::vec3 falloff;
    Light(glm::vec3 pos, glm::vec3 color, glm::vec3 falloff);
    virtual double getArea();
    virtual glm::vec3 getIntensity(Ray lightRay);
    virtual std::vector<Ray> getRays(glm::vec3 point);
};

std::ostream& operator<<(std::ostream& out, const Light& l);

#include <iostream>

#include <glm/ext.hpp>
#include "AreaLight.hpp"

AreaLight::AreaLight(glm::vec3 pos, double w, double h, glm::vec3 color, double falloff[3]) : Light(pos, color, falloff), width(w), height(h) {}

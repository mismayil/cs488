#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include "Light.hpp"

class AreaLight : public Light {
public:
  AreaLight(glm::vec3 pos, double w, double h, glm::vec3 color, double falloff[3]);
  double width;
  double height;
};

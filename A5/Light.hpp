#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

class Light {
public:
  Light(glm::vec3 pos, glm::vec3 color, double falloff[3]);
  glm::vec3 colour;
  glm::vec3 position;
  double *falloff;
};

std::ostream& operator<<(std::ostream& out, const Light& l);

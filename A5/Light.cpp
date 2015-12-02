#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light(glm::vec3 pos, glm::vec3 color, double falloff[3]) : position(pos), colour(color), falloff(falloff) {}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << glm::to_string(l.colour)
  	  << ", " << glm::to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}

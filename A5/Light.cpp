#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light(glm::vec3 pos, glm::vec3 color, glm::vec3 falloff) : position(pos), colour(color), falloff(falloff) {}

double Light::getArea() { return 1; }

std::vector<softRay> Light::getRays(glm::vec3 point, double tao) {
    Ray lightRay(position, normalize(position - point));
    Ray shadowRay(point, (EPS + tao) * lightRay.d);
    std::vector<softRay> s;
    s.push_back({lightRay, shadowRay});
    return s;
}

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

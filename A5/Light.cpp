#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light(glm::vec3 pos, glm::vec3 color, glm::vec3 falloff) : position(pos), colour(color), falloff(falloff) {}

double Light::getArea() { return 1; }

std::vector<Ray> Light::getRays(glm::vec3 point) {
    glm::vec3 direction = normalize(position - point);
    Ray lightRay(point + EPS * direction, direction);
    std::vector<Ray> v;
    v.push_back(lightRay);
    return v;
}

glm::vec3 Light::getIntensity(Ray lightRay) {
    double distance = glm::length(lightRay.d);
    return colour / (falloff[0] + falloff[1] * distance + falloff[2] * pow(distance, 2));
}

int Light::getSamples() { return 1; }

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
